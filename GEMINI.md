# Wireless Weightlifting Trolley - Project Overview

**Developer:** P.Sannith (CSE 3rd Year)

This project focuses on a remotely controlled, wireless weightlifting trolley prototype designed for mechanical material handling. It uses an ESP32 for WiFi control and a 6-relay H-Bridge array for high-power 24V motor management.

## Core Technologies & Components
*   **Microcontroller:** ESP32 DevKit V1 (WiFi Enabled).
*   **Actuators:** 3x 24V 5840-31ZY High Torque Worm Gear Motors (1x Lift, 2x Navigation). *(Currently testing with 5V motors).*
*   **Power Supply:** 24V 15A 360W SMPS.
*   **Logic Power:** 1x LM2596 Buck Converter (Stepping 24V down to exactly 5.0V).
*   **Motor Control:** 1x 4-Channel 5V Relay Module + 1x 2-Channel 5V Relay Module (Total 6 Relays).
*   **Safety:** Self-locking worm gears and emergency software stop.

## Hardware Architecture (Relay H-Bridge)
To control motor direction (Forward/Reverse), relays are wired in an **H-Bridge Configuration**:
1.  **NO (Normally Open):** Connect to Positive Power (5V for testing, 24V final) on all relays.
2.  **NC (Normally Closed):** Connect to Common Ground (GND) on all relays.
3.  **COM (Common):** Connect to the Motor wires (2 relays per motor).

### Power Distribution
1.  **High Power:** SMPS output connects directly to Relay NO/NC terminals via H-bridge bridge wires.
2.  **Logic Power:** SMPS -> LM2596 Buck Converter (Tuned to 5.0V) -> ESP32 VIN & VCC of both Relay Modules.
3.  **Common Ground:** All GNDs (SMPS, Buck, ESP32, both Relay Modules) MUST be connected together.

### Control Signal Mapping (ESP32)
*   **Lifting Motor (4-Channel or 2-Channel Module):** GPIO 12 (UP), GPIO 13 (DOWN).
*   **Left Navigation (4-Channel Module):** GPIO 14 (FWD), GPIO 27 (BWD).
*   **Right Navigation (2-Channel Module):** GPIO 26 (FWD), GPIO 25 (BWD).

## 8-Directional Joystick Mapping
*   **Forward:** Both motors move forward.
*   **Backward:** Both motors move backward.
*   **Right Pivot:** Left motor forward, Right motor stopped.
*   **Right In-Place:** Left motor forward, Right motor backward.
*   **Left Pivot:** Right motor forward, Left motor stopped.
*   **Left In-Place:** Right motor forward, Left motor backward.
*   **Diagonal Back-Right:** Right motor backward, Left motor stopped.
*   **Diagonal Back-Left:** Left motor backward, Right motor stopped.

## Current Control Code (Updated with Logic Toggle)
```cpp
#include <WiFi.h>
#include <WebServer.h>

// ================= CONFIGURATION =================
// Set this to false if using single-channel relay modules (usually Active High)
// Set this to true if using multi-channel relay modules (usually Active Low)
#define RELAY_ACTIVE_LOW true 

const int RELAY_ON  = RELAY_ACTIVE_LOW ? LOW : HIGH;
const int RELAY_OFF = RELAY_ACTIVE_LOW ? HIGH : LOW;

const int liftUpPin = 12; const int liftDownPin = 13;
const int leftFwdPin = 14; const int leftBwdPin = 27;
const int rightFwdPin = 26; const int rightBwdPin = 25;
// =================================================

WebServer server(80);

const char* htmlPage = R"rawliteral(
... (HTML Content Remains Same) ...
)rawliteral";

void handleRoot() { server.send(200, "text/html", htmlPage); }

void stopLift() { 
  Serial.println("LIFT: STOP");
  digitalWrite(liftUpPin, RELAY_OFF); digitalWrite(liftDownPin, RELAY_OFF); 
  server.send(200); 
}

void stopNav() { 
  Serial.println("NAV: STOP");
  digitalWrite(leftFwdPin, RELAY_OFF); digitalWrite(leftBwdPin, RELAY_OFF); 
  digitalWrite(rightFwdPin, RELAY_OFF); digitalWrite(rightBwdPin, RELAY_OFF); 
  server.send(200); 
}

// Navigation & Lifting functions use RELAY_ON / RELAY_OFF
// ... (Refer to Trolley_Pro.ino for full implementation) ...

void setup() {
  Serial.begin(115200);
  int pins[] = {12, 13, 14, 27, 26, 25};
  for(int p : pins) { pinMode(p, OUTPUT); digitalWrite(p, RELAY_OFF); }
  WiFi.softAP("Trolley_Pro", "12345678");
  server.begin();
}
void loop() { server.handleClient(); }
```

## Known Troubleshooting & Fixes
*   **Lifting Not Working (Single Relays):** If you are using independent single-channel relay modules, they are likely **Active High**. In the code, change `#define RELAY_ACTIVE_LOW true` to `false`.
*   **Relay Logic Test:** Open the **Serial Monitor (115200 baud)**. When you press "LIFT UP", you should see `LIFT: UP` in the monitor. If you see the message but the relay doesn't click, check your wiring and logic toggle.
*   **Upload Errors (Semaphore Timeout / Port missing):** Requires the **Silicon Labs CP210x USB to UART Bridge** driver. Do not select Bluetooth COM ports.
*   **Connecting... Error during upload:** Press and hold the **"BOOT"** button on the ESP32 when the IDE says `Connecting......._____`.
*   **Silent Relays:** If a relay doesn't click, verify the `JD-VCC` to `VCC` jumper is installed (for multi-channel modules), and High/Low trigger (if applicable) is set correctly.

## Assembly Checklist
1. [x] Install CP210x Drivers and test ESP32 code upload via USB.
2. [ ] Calibrate Buck Converter to exactly 5.0V.
3. [ ] Connect common grounds across all modules.
4. [ ] Wire H-Bridge (NO to Power, NC to GND, COM to Motors).
5. [ ] Connect ESP32 GPIOs to Relay IN pins.
6. [ ] Upload Code (holding BOOT button) and test WiFi connection (`192.168.4.1`).
