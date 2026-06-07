#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Pin Definitions ---
#define DHTPIN        5
#define DHTTYPE       DHT22
#define RED_LED_PIN   19
#define GREEN_LED_PIN 23
#define I2C_SDA       22
#define I2C_SCL       21

// --- Thresholds (non-const so web buttons can change them) ---
// HIGH setting: 33C / 50% | LOW setting: 15C / 49%
float TEMP_THRESHOLD = 33.0;
float HUM_THRESHOLD  = 50.0;
bool  threshHigh     = true; // tracks which setting is active

// --- Network & HiveMQ Cloud ---
const char* ssid        = "Thebe";
const char* password    = " Thiza16321401@";
const char* mqtt_server = "6da3042b9182445eaa27656920faacb3.s1.eu.hivemq.cloud";
const int   mqtt_port   = 8883;
const char* mqtt_user   = "esp32_node";
const char* mqtt_pass   = "SecurePass123!";

// --- Instances ---
DHT               dht(DHTPIN, DHTTYPE);
WiFiClientSecure  espClient;
PubSubClient      mqttClient(espClient);
WebServer         server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- State ---
float temp = 0.0, hum = 0.0;
bool  redLed = false, greenLed = true;
bool  autoMode = true;
unsigned long lastSample = 0;

// -------------------------------------------------------
// HELPERS
// -------------------------------------------------------
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temp, 1); lcd.print("C H:"); lcd.print(hum, 0); lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(autoMode ? "AUTO " : "MAN  ");
  lcd.print(threshHigh ? "TH:HIGH" : "TH:LOW ");
}

void evaluateThresholds() {
  bool alert = (temp > TEMP_THRESHOLD || hum > HUM_THRESHOLD);
  redLed   = alert;
  greenLed = !alert;
  digitalWrite(RED_LED_PIN,   redLed   ? HIGH : LOW);
  digitalWrite(GREEN_LED_PIN, greenLed ? HIGH : LOW);
}

// -------------------------------------------------------
// MQTT CALLBACK
// Commands: LED_ON | LED_OFF | LED_TOGGLE | MODE_AUTO
//           THRESH_HIGH | THRESH_LOW
// -------------------------------------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  if (msg == "MODE_AUTO") {
    autoMode = true;
    evaluateThresholds();
  } else if (msg == "THRESH_HIGH") {
    // HIGH threshold: 33C and 50%
    TEMP_THRESHOLD = 33.0; HUM_THRESHOLD = 50.0; threshHigh = true;
    if (autoMode) evaluateThresholds();
  } else if (msg == "THRESH_LOW") {
    // LOW threshold: 15C and 49%
    TEMP_THRESHOLD = 15.0; HUM_THRESHOLD = 49.0; threshHigh = false;
    if (autoMode) evaluateThresholds();
  } else {
    autoMode = false;
    if      (msg == "LED_ON")     { redLed = true;  digitalWrite(RED_LED_PIN, HIGH); }
    else if (msg == "LED_OFF")    { redLed = false; digitalWrite(RED_LED_PIN, LOW);  }
    else if (msg == "LED_TOGGLE") { redLed = !redLed; digitalWrite(RED_LED_PIN, redLed ? HIGH : LOW); }
  }

  updateLCD();
  String ack = "{\"mode\":\"" + String(autoMode ? "AUTO" : "MANUAL") + "\",\"thresh\":\"" + String(threshHigh ? "HIGH" : "LOW") + "\",\"red\":\"" + String(redLed ? "ON" : "OFF") + "\"}";
  mqttClient.publish("esp32/node/ack", ack.c_str());
}

// -------------------------------------------------------
// MQTT RECONNECT
// -------------------------------------------------------
void mqttReconnect() {
  if (mqttClient.connected()) return;
  static unsigned long lastAttempt = 0;
  if (millis() - lastAttempt < 5000) return;
  lastAttempt = millis();
  String clientId = "ESP32-" + WiFi.macAddress();
  if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
    Serial.println("Connected to HiveMQ Cloud!");
    mqttClient.publish("esp32/node/status", "online");
    mqttClient.subscribe("esp32/node/command");
  } else {
    Serial.print("MQTT failed rc="); Serial.println(mqttClient.state());
  }
}

// -------------------------------------------------------
// WEB SERVER — dashboard with threshold buttons
// -------------------------------------------------------
void handleRoot() {
  String th = threshHigh ? "HIGH (33C / 50%)" : "LOW (15C / 49%)";
  String html = "<html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;margin:20px} button{padding:10px 16px;margin:4px;font-size:14px;cursor:pointer}";
  html += ".high{background:#e53935;color:white} .low{background:#1e88e5;color:white} .auto{background:#43a047;color:white}</style></head>";
  html += "<body><h2>Smart Oven Controller</h2>";
  html += "<p>Mode: <b style='color:" + String(autoMode ? "green" : "orange") + "'>" + String(autoMode ? "AUTO" : "MANUAL") + "</b></p>";
  html += "<p>Threshold Setting: <b>" + th + "</b></p>";
  html += "<p>Temperature: <b>" + String(temp, 1) + " C</b> | Humidity: <b>" + String(hum, 1) + " %</b></p>";
  html += "<p>Red LED: "   + String(redLed   ? "ON" : "OFF") + " | Green LED: " + String(greenLed ? "ON" : "OFF") + "</p>";
  html += "<hr><h3>Threshold Control</h3>";
  // THE TWO BUTTONS THE BRIEF ASKS FOR
  html += "<a href='/thresh/high'><button class='high'>Set HIGH (33C / 50%)</button></a> ";
  html += "<a href='/thresh/low'><button class='low'>Set LOW (15C / 49%)</button></a>";
  html += "<hr><h3>LED Control</h3>";
  html += "<a href='/led/on'><button class='high'>Red ON</button></a> ";
  html += "<a href='/led/off'><button>Red OFF</button></a> ";
  html += "<a href='/mode/auto'><button class='auto'>Auto Mode</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Threshold route handlers
void handleThreshHigh() {
  TEMP_THRESHOLD = 33.0; HUM_THRESHOLD = 50.0; threshHigh = true;
  if (autoMode) evaluateThresholds();
  updateLCD();
  server.sendHeader("Location", "/"); server.send(302);
}
void handleThreshLow() {
  TEMP_THRESHOLD = 15.0; HUM_THRESHOLD = 49.0; threshHigh = false;
  if (autoMode) evaluateThresholds();
  updateLCD();
  server.sendHeader("Location", "/"); server.send(302);
}

// LED route handlers
void handleLedOn()   { autoMode=false; redLed=true;  digitalWrite(RED_LED_PIN,HIGH); updateLCD(); server.sendHeader("Location","/"); server.send(302); }
void handleLedOff()  { autoMode=false; redLed=false; digitalWrite(RED_LED_PIN,LOW);  updateLCD(); server.sendHeader("Location","/"); server.send(302); }
void handleModeAuto(){ autoMode=true;  evaluateThresholds(); updateLCD(); server.sendHeader("Location","/"); server.send(302); }

// -------------------------------------------------------
// SETUP
// -------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n--- Smart Oven Controller Boot ---");

  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init(); lcd.backlight();
  lcd.print("Booting...");

  pinMode(RED_LED_PIN,   OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN,   LOW);

  dht.begin();

  WiFi.disconnect(true); delay(500);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lcd.setCursor(0,1); lcd.print("WiFi...");

  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 15000) { delay(500); Serial.print("."); }
  Serial.println();

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP: "); Serial.println(WiFi.localIP());
    lcd.setCursor(0,0); lcd.print("WiFi Connected!");
    lcd.setCursor(0,1); lcd.print(WiFi.localIP().toString());
  } else {
    lcd.print("WiFi Failed!");
    Serial.println("WiFi Failed.");
  }
  delay(3000);

  server.on("/",            handleRoot);
  server.on("/thresh/high", handleThreshHigh); // exam spec button 1
  server.on("/thresh/low",  handleThreshLow);  // exam spec button 2
  server.on("/led/on",      handleLedOn);
  server.on("/led/off",     handleLedOff);
  server.on("/mode/auto",   handleModeAuto);
  server.begin();
  Serial.println("Web Server Started.");

  espClient.setInsecure();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
}

// -------------------------------------------------------
// LOOP
// -------------------------------------------------------
void loop() {
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED) { mqttReconnect(); mqttClient.loop(); }

  if (millis() - lastSample > 3000) {
    lastSample = millis();
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (isnan(t) || isnan(h)) {
      Serial.println("Sensor Error!");
      lcd.clear(); lcd.print("Sensor Error!");
      return;
    }
    temp = t; hum = h;

    if (autoMode) evaluateThresholds();
    updateLCD();

    Serial.print("T:"); Serial.print(temp,1); Serial.print("C H:");
    Serial.print(hum,1); Serial.print("% Thresh:");
    Serial.println(threshHigh ? "HIGH" : "LOW");

    String payload = "{\"temp\":"+String(temp,1)+",\"hum\":"+String(hum,1)+
                     ",\"thresh\":\""+String(threshHigh?"HIGH":"LOW")+
                     "\",\"auto\":"+String(autoMode?"true":"false")+"}";
    if (mqttClient.connected()) mqttClient.publish("esp32/node/telemetry", payload.c_str());
  }
}