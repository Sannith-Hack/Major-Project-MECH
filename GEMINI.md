# Wireless Weightlifting Trolley - Project Overview

This project focuses on a remotely controlled, wireless weightlifting trolley prototype designed for mechanical material handling. It uses an ESP32 for WiFi control and a 6-relay H-Bridge array for high-power 24V motor management.

## Core Technologies & Components

*   **Microcontroller:** ESP32 (WiFi Enabled).
*   **Actuators:** 3x 24V 5840-31ZY High Torque Worm Gear Motors (1x Lift, 2x Navigation).
*   **Power Supply:** 24V 15A 360W SMPS.
*   **Logic Power:** 1x LM2596 Buck Converter (Stepping 24V down to exactly 5V).
*   **Motor Control:** 1x 4-Channel 5V Relay Module + 1x 2-Channel 5V Relay Module (Total 6 Relays).
*   **Safety:** Self-locking worm gears and emergency software stop.

## Hardware Architecture (Relay H-Bridge)

### Power Distribution
1.  **High Power (24V):** SMPS 24V output connects directly to the Relay "Normally Open" (NO) terminals of both modules.
2.  **Logic Power (5V):** SMPS 24V -> LM2596 Buck Converter (Tuned to 5.0V) -> ESP32 VIN & VCC of both Relay Modules.
3.  **Common Ground:** All GNDs (SMPS, Buck, ESP32, both Relay Modules) MUST be connected together.

### Control Signal Mapping (ESP32)
*   **Lifting Motor (4-Channel Module):** GPIO 12 (UP), GPIO 13 (DOWN).
*   **Left Navigation (4-Channel Module):** GPIO 14 (FWD), GPIO 27 (BWD).
*   **Right Navigation (2-Channel Module):** GPIO 26 (FWD), GPIO 25 (BWD).

## Software & Programming
The ESP32 hosts a mobile-friendly web server.
*   **Access Point:** `Trolley_Pro` (IP: `192.168.4.1`).
*   **Logic:** Active Low (LOW = Relay ON).

## Safety Protocols
*   **Voltage Calibration:** Use a multimeter to set the Buck Converter to 5V *before* connecting the ESP32.
*   **Mechanical Isolation:** Ensure 24V motor wires (16 AWG) are physically separated from 5V logic jumpers.
*   **Emergency Stop:** Software includes a global "STOP ALL" function.
