# Smart Home Automation: Secure IoT Smart Oven Controller

## Project Overview
This repository contains the firmware and architecture design for an industrial-grade, cloud-connected Smart Oven Controller. Engineered around the ESP32 microcontroller, the system implements localized real-time safety automation thresholds while maintaining an encrypted telemetry pipeline to remote cloud brokers. The project bridges low-level hardware communication with scalable cloud infrastructure, demonstrating a complete end-to-end Industry 4.0 IoT implementation.

## Features
* **Asynchronous State Machine:** Multi-threaded firmware architecture designed for zero-latency sensor polling and immediate safety override execution.
* **Environmental & Hazard Monitoring:** Real-time logging of ambient temperature, humidity levels, and hazardous gas thresholds.
* **Periphery Hardware Feedback:** Visual system state reporting via a physical Liquid Crystal Display (LCD) utilizing localized communication buses.
* **Secure Telemetry Pipeline:** Structured JSON data serialization routed over authenticated MQTT protocols to remote dashboards.
* **Fault Detection & Bring-Up Logic:** Integrated edge-case validation and hardware serial logging for structural maintenance and debugging.

---

## Hardware Architecture & Peripherals

The system architecture utilizes the ESP32 platform to handle simultaneous digital/analog sensor processing and network stack execution.

### Pin Mapping & Peripheral Interfaces

| Component | Interface Type | Microcontroller Pin (ESP32 GPIO) | Description |
| :--- | :--- | :--- | :--- |
| **DHT22 Sensor** | 1-Wire Digital | GPIO 15 | Ambient temperature and humidity monitoring. |
| **MQ-2 Gas Sensor** | Analog Input | GPIO 34 (ADC1_CH6) | Hazardous gas/smoke detection threshold analysis. |
| **Ultrasonic Sensor (Trigger)** | Digital Output | GPIO 13 | High-frequency pulse emission for distance tracking. |
| **Ultrasonic Sensor (Echo)** | Digital Input | GPIO 12 | Pulse reflection timing calculation. |
| **I2C LCD Display (SDA)** | I2C Serial Data | GPIO 22 | Serial data line for local hardware readouts. |
| **I2C LCD Display (SCL)** | I2C Serial Clock | GPIO 21 | Serial clock line for bus synchronization. |
| **Active Buzzer** | Digital Output | GPIO 2 | Audible local alarm for critical safety overrides. |

---

## Technical Specifications & Code Framework

### Firmware Environment
* **Language:** C/C++ (Arduino Core / Espressif ESP-IDF framework)
* **Libraries Utilized:** `WiFi.h`, `BlynkSimpleEsp32.h`, `DHT.h`, `Wire.h`, `LiquidCrystal_I2C.h`

### Core Firmware Methodology
1. **Hardware Bring-Up & Validation:** The `setup()` routine initializes the I2C bus at a standard 100 kHz clock speed, configures internal pull-up resistors for the GPIO inputs, establishes a secure Wi-Fi handshake, and performs a localized self-test on sensory components.
2. **Non-Blocking Execution:** To prevent CPU starvation and ensure safety metrics are evaluated instantaneously, the main execution loop avoids blocking delays (`delay()`). It relies on standard internal timing counters (`millis()`) to multiplex network synchronization and sensor sampling.
3. **Data Serialization:** Sensor inputs are parsed, converted into standardized float/integer values, and packed into compressed data frames for efficient network transmission.

---

## Cloud & Network Integration

The controller interfaces directly with cloud infrastructure to provide remote monitoring and control capabilities while maintaining secure access perimeters.

* **Network Protocol:** Wi-Fi 802.11 b/g/n utilizing WPA2 personal security standards.
* **Data Transport Layer:** Telemetry streaming via a secure cloud broker interface, utilizing persistent TCP links.
* **Remote Command Ingestion:** Asynchronous callback functions handle incoming cloud-to-device parameters (such as updating threshold targets or executing forced safety shutdowns remotely).

---

## Deployment & Verification

### Prerequisites
* Arduino IDE (v2.0 or later) or VS Code with the PlatformIO extension.
* Espressif ESP32 Core toolkit installed.

### Compilation and Flashing
1. Clone this repository to your local directory:
   ```bash
   git clone https://github.com/ThebeLedwaba/smart-oven-controller.git
   ```
2. Open the source directory in your preferred IDE.
3. Update the network credentials profile with your local access parameters:
   ```cpp
   const char* ssid        = "YOUR_WIFI_SSID";
   const char* password    = "YOUR_WIFI_PASSWORD";
   const char* mqtt_server = "YOUR_MQTT_BROKER_ADDRESS";
   const char* mqtt_user   = "YOUR_MQTT_USERNAME";
   const char* mqtt_pass   = "YOUR_MQTT_PASSWORD";
   ```
4. Target the appropriate board manager profile (ESP32 Dev Module).
5. Compile the code stack and flash the binary to the destination hardware via a serial interface.

### Testing and Debugging
System verification can be monitored via any hardware-interfaced Serial Monitor configured at a baud rate of 115200. The output log streams comprehensive boot validation telemetry, network connection states, sensor status updates, and fault conditions.

**Example Serial Output:**
```
[INFO] Initializing Hardware Peripherals...
[INFO] I2C Bus Established at Address 0x27
[INFO] Connecting to Local Access Point...
[INFO] Network Connected. IP Address Assigned: 192.168.1.105
[INFO] Core Automated Systems Active. System Status: Nominal.
```

---

## Author
ThebeLedwaba

## License
This project is open source.
 
