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
 
---
 
## Dataset
 
**Eskom National Electricity Grid Dataset (ESK2033.csv)**
 
- **Records:** 43,824 hourly observations (April 2018 – March 2023)
- **Features:** 42 columns covering generation by source, demand forecasts, renewable energy output, installed capacity, load shedding events, and planned/unplanned outages
- **Target Variable:** `RSA Contracted Demand` (MW) — total actual electricity demand contracted across South Africa per hour
- **Source:** Eskom official grid data (not included in this repo due to licensing)
> To reproduce this project, obtain the dataset from Eskom's official data portal and place `ESK2033.csv` in the root directory.
 
---
 
## Problem Statement
 
Accurate electricity demand forecasting is critical for grid operators to manage generation dispatch, prevent load shedding, and balance supply with demand in real time. This project builds and tunes a neural network regression model to predict hourly RSA Contracted Demand from 40 grid features, simulating the kind of ML work done in energy analytics roles.
 
---
 
## Methodology
 
### Preprocessing
- Dropped rows with missing values (NaN)
- Removed the datetime index column
- Separated features (X) from target (y = RSA Contracted Demand)
- Split: 70% training / 15% validation / 15% test
- Scaled all input features using `StandardScaler` (fit on training set only)
### Experiment Design
Each of the 20 models changes at least one hyperparameter from the previous configuration. The hyperparameters explored include:
 
| Hyperparameter | Values Tested |
|---|---|
| Neurons per layer | 64, 128, 256 |
| Hidden layers | 2, 3 |
| Activation function | ReLU, ELU, SELU |
| Optimiser | Adam, RMSprop |
| Batch size | 16, 32, 64 |
| Epochs | 200, 500, 1000 |
| Dropout rate | 0.1, 0.2 |
| Learning rate schedule | ReduceLROnPlateau |
| Early stopping | patience=30 |
 
---
 
## Results Summary
 
| Rank | Model | Test MAE (MW) | Val MAE (MW) | Key Change |
|------|-------|--------------|--------------|------------|
| 1 | Model 14 | **60.57** | 62.74 | Early stopping, ELU, batch 16 |
| 2 | Model 6  | **60.58** | 61.54 | ELU activation ← biggest single gain |
| 3 | Model 11 | 60.74 | 61.51 | LR schedule |
| 4 | Model 15 | 60.75 | 63.50 | SELU activation |
| 5 | Model 9  | 60.92 | 63.46 | 500 epochs |
| ... | ... | ... | ... | ... |
| 20 | Model 7  | 72.56 | 75.50 | RMSprop ← worst |
 
**Baseline (Model 1):** 65.74 MW Test MAE  
**Best (Model 14):** 60.57 MW Test MAE  
**Improvement:** 8% over baseline
 
---
 
## Key Findings
 
**Most impactful positive change:** Switching from ReLU to ELU activation (Model 6) produced the largest single improvement — dropping Test MAE from 63.67 MW to 60.58 MW. ELU allows small negative outputs rather than clamping at zero, which prevents dying neurons and maintains better gradient flow throughout training.
 
**Most negative change:** RMSprop optimiser (Model 7) was the worst configuration at 72.56 MW Test MAE. Adam's momentum and adaptive per-parameter learning rates made it significantly better suited to this high-dimensional regression task.
 
**Dropout was consistently harmful:** Both Dropout(0.2) and Dropout(0.1) increased Test MAE. Eskom's grid features are strongly physically correlated with demand — randomly deactivating neurons breaks these correlations and hurts learning.
 
**Depth didn't help:** Three-layer architectures (Models 3, 4, 19) consistently underperformed the two-layer 128-128 setup. For tabular regression with ~14,000 training samples, additional depth adds optimisation complexity without improving representational capacity.
 
---
 
## Project Structure
 
```
neural-network-hyperparameter-optimisation/
│
├── 219119007_Ledwaba_T_AI_HOMEWORK2_FINAL.ipynb   # Main notebook (20 models)
└── README.md
```
 
---
 
## Tech Stack
 
- **Python 3.13**
- **TensorFlow 2.21 / Keras 3.14** — model building and training
- **scikit-learn** — preprocessing (StandardScaler, train_test_split, MAE)
- **pandas** — data manipulation and results summary
- **matplotlib** — loss curves, MAE curves, scatter plots
- **Google Colab (T4 GPU)** — training environment
---
 
## How to Run
 
```bash
# Clone the repo
git clone https://github.com/ThebeLedwaba/neural-network-hyperparameter-optimisation.git
cd neural-network-hyperparameter-optimisation
 
# Install dependencies
pip install tensorflow pandas scikit-learn matplotlib jupyter
 
# Place ESK2033.csv in the root directory
# Update the file_path variable in the notebook to point to your CSV
 
# Launch Jupyter
jupyter notebook
```
 
> **Recommended:** Run on Google Colab with a T4 GPU for faster training. Upload `ESK2033.csv` to `/content/` and update the file path accordingly.
 
---
 
## Author
 
**Thebe Ledwaba**  
Computer Systems Engineering (Advanced Diploma, NQF 7)  
Vaal University of Technology  
 
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?logo=linkedin)](https://linkedin.com/in/thebe-ledwaba)
[![GitHub](https://img.shields.io/badge/GitHub-ThebeLedwaba-black?logo=github)](https://github.com/ThebeLedwaba)
[![Portfolio](https://img.shields.io/badge/Portfolio-thebeledwabawebsite.netlify.app-brightgreen)](https://thebeledwabawebsite.netlify.app)
 
---
 
*Part of an ongoing portfolio in ML/Data Science, Embedded Systems, and Full-Stack Development.*
