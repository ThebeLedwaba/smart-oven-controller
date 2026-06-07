# Smart Oven Controller

An ESP32-based IoT project for monitoring and controlling oven temperature and humidity using MQTT cloud communication and web server interface.

## Features

- **Temperature & Humidity Monitoring**: Real-time sensor readings using DHT22
- **MQTT Cloud Integration**: Connects to HiveMQ Cloud for remote monitoring
- **Web Server Interface**: Control settings via HTTP web server
- **LCD Display**: 16x2 I2C LCD display for local status feedback
- **Dual LED Indicators**: Red (heating) and Green (normal) status LEDs
- **Adjustable Thresholds**: Two preset settings - HIGH (33°C/50%) and LOW (15°C/49%)
- **Auto Mode**: Automatic control based on temperature and humidity thresholds

## Hardware Requirements

- ESP32 microcontroller
- DHT22 temperature/humidity sensor
- 16x2 I2C LCD display
- Red and Green LEDs
- WiFi connectivity

## Pin Configuration

| Component | GPIO Pin |
|-----------|----------|
| DHT22 Data | GPIO 5 |
| Red LED | GPIO 19 |
| Green LED | GPIO 23 |
| I2C SDA | GPIO 22 |
| I2C SCL | GPIO 21 |

## Network Configuration

Before uploading, update the following in the sketch:
- `ssid`: Your WiFi network name
- `password`: Your WiFi password
- `mqtt_server`: HiveMQ Cloud broker address
- `mqtt_user`: MQTT username
- `mqtt_pass`: MQTT password

## Installation

1. Install the required Arduino libraries:
   - WiFi.h (built-in)
   - WebServer.h (built-in)
   - PubSubClient
   - DHT
   - LiquidCrystal_I2C
   - Wire (built-in)

2. Update network credentials in the sketch

3. Upload to ESP32 using Arduino IDE

## Usage

- **Local Control**: Access the web server on the ESP32's IP address
- **Remote Monitoring**: Connect via MQTT to HiveMQ Cloud
- **Manual Mode**: Toggle between auto and manual control
- **Threshold Settings**: Switch between HIGH and LOW presets using web interface

## Threshold Settings

| Mode | Temperature | Humidity |
|------|-------------|----------|
| HIGH | 33°C | 50% |
| LOW | 15°C | 49% |

## Status Indicators

- **Green LED**: Normal operation
- **Red LED**: Heating mode active
- **LCD Display**: Shows current temperature and humidity

## License

This project is open source.

## Author

ThebeLedwaba
