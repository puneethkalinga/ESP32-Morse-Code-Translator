# IoT Assistive Morse Code Communicator & Real-Time Dashboard

[![ESP32](https://img.shields.io/badge/Hardware-ESP32-E7352C?logo=espressif&logoColor=white)](#)
[![C++](https://img.shields.io/badge/Firmware-C%2B%2B-00599C?logo=c%2B%2B&logoColor=white)](#)
[![WebSockets](https://img.shields.io/badge/Protocol-WebSockets-green)](#)
[![HTML5 / JS](https://img.shields.io/badge/Frontend-Vanilla%20JS%20%7C%20HTML5-F7DF1E?logo=javascript&logoColor=black)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

An IoT-driven assistive technology device engineered to enable individuals with severe motor or speech impairments to communicate by converting simple tactile switch/Morse code inputs into natural English text in real time.

---

## 📌 System Architecture

```
[ Tactile Input / Switch ]
          │
          ▼
    [ ESP32 MCU ] 
    ├── Morse Decoder Algorithm (Dots, Dashes, Intra-char timing)
    ├── Audio-Visual Feedback (Active Buzzer & Status LED)
    └── Wi-Fi Station Mode (AsyncWebServer & WebSockets)
          │ (Sub-50ms Low-Latency Stream)
          ▼
 [ Web Monitoring Dashboard ] 
    ├── Real-Time Decoded Character & Word Stream
    ├── Audio-Visual Status Confirmation
    └── Persistent Message History (Browser LocalStorage)
```

## ✨ Key Highlights

- **Real-Time Translation Engine:** Decodes duration-based switch pulses into dots (`.`) and dashes (`-`) with dynamic threshold calibration.
- **Ultra-Low Latency:** Uses lightweight asynchronous WebSockets for sub-50ms message display on connected web dashboards.
- **Assistive Dual Feedback:** Emits synchronized audio tones via buzzer and visual LED pulses to validate each stroke to the user.
- **Persistent Message Ledger:** Automatically logs all decoded messages locally in browser storage for session recovery.

## 🛠️ Hardware & Pin Configuration

| Component | ESP32 GPIO Pin | Description |
| :--- | :--- | :--- |
| **Tactile Push Button** | `GPIO 4` | Active-low / Active-high input switch with internal pull-up |
| **Buzzer** | `GPIO 18` | Audio tone generator for dot/dash feedback |
| **Status LED** | `GPIO 2` | Onboard or external visual indicator |

## 🚀 Getting Started

### 1. Hardware Assembly
Connect the tactile button to `GPIO 4`, buzzer to `GPIO 18`, and LED to `GPIO 2` as outlined in the pin configuration table above.

### 2. Flashing Firmware
1. Open `firmware/morse_translator.ino` in the Arduino IDE.
2. Under **Tools > Board**, select **ESP32 Dev Module**.
3. Install required libraries:
   - `ESPAsyncWebServer`
   - `AsyncTCP`
4. Enter your Wi-Fi credentials in `morse_translator.ino`:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
5. Click **Upload**. Open Serial Monitor at **115200 baud** to view the assigned local IP address.

### 3. Running Web Dashboard
Open `web/index.html` in any browser on the same Wi-Fi network and enter the ESP32 IP address to view live translations.

---

**Author:** [Kalinga Puneeth Kumar](https://github.com/puneethkalinga)  
Department of Artificial Intelligence, Amrita Vishwa Vidyapeetham, Coimbatore
