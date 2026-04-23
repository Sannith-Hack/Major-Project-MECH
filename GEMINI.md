# Wireless Weightlifting Trolley - Project Overview

This project focuses on the development of a remotely controlled, wireless weightlifting machine (Trolley) designed for mechanical material handling. It integrates high-torque DC motors, an ESP32 microcontroller for wireless connectivity, and a robust power management system.

## Core Technologies & Components

*   **Microcontroller:** ESP32 (WiFi + Bluetooth enabled).
*   **Actuators:** 24V 27RPM High Torque DC Worm Gear Motors (Model: 5840-31ZY) with self-locking capabilities.
*   **Power Supply:** 24V 15A 360W SMPS (Switching Power Supply).
*   **Motor Control:** High-current H-Bridge drivers (e.g., BTS7960 43A).
*   **Power Regulation:** DC-DC Buck Converter (Stepping down 24V to 5V for logic circuits).
*   **Safety Logic:** 4-Channel Relay Module for high-power switching and safety interlocks.

## Hardware Architecture & Wiring

### Power Distribution
1.  **High Power:** SMPS 24V output directly feeds the Motor Driver power inputs.
2.  **Logic Power:** SMPS 24V output connects to the Buck Converter input.
3.  **Low Power:** Buck Converter (tuned to 5V) powers the ESP32 (via VIN) and the Relay Module.

### Control Signal Mapping (ESP32)
*   **GPIO 12:** Motor Forward / Lift Up (PWM).
*   **GPIO 13:** Motor Backward / Lower Down (PWM).
*   **GND:** All grounds (SMPS, Buck, ESP32, Driver) MUST be common.

## Software & Programming

The ESP32 is programmed using the **Arduino IDE**.

### Key Commands / Setup
*   **Board Manager URL:** `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
*   **Board Selection:** `DOIT ESP32 DEVKIT V1`.
*   **Communication:** Serial Baud Rate `115200`.

### Prototype Testing Logic
The default testing firmware establishes a WiFi Access Point (`Trolley_Control`). A web server hosted on the ESP32 provides a mobile-friendly browser interface at `192.168.4.1` for basic Up/Down/Stop controls.

## Safety Protocols

*   **Self-Locking Gears:** The worm gear motors prevent the load from dropping even if power is lost.
*   **Isolation:** Use of optocouplers is recommended to isolate the ESP32 from motor back-EMF.
*   **Mechanical Limits:** TODO: Integrate limit switches to prevent mechanical over-travel.
*   **Initial Testing:** Always test motor direction and WiFi connectivity without the mechanical load attached.

## Directory Contents

*   `USEFULL/`: Contains CAD models and technical reference images of components.
*   `Doc.txt`: Detailed technical notes on wiring, safety, and code snippets.
*   `Weightlifting_Trolley_Final_Report.docx`: Comprehensive project documentation.
*   `generate_final_report.py`: Utility script used to generate project reports.
