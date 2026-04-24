# Wireless Weightlifting Trolley - Complete Setup Guide (Joystick Edition)

This document is the master setup guide for the ESP32-based Wireless Weightlifting Trolley, updated with the Virtual Joystick and Touch Support.

---

## 1. Software Setup & Code Upload

### 1.1 Driver Installation
The ESP32 DevKit V1 requires specific drivers to communicate over USB.
1. Download the **"CP210x USB to UART Bridge VCP Drivers"** from the Silicon Labs website.
2. Install the driver and restart the Arduino IDE.
3. Open Windows Device Manager -> **Ports (COM & LPT)** to verify it appears as `Silicon Labs CP210x USB to UART Bridge (COM X)`.

### 1.2 Uploading the Code
1. Disconnect any external 5V/24V power from the ESP32. **Use ONLY the USB cable** while uploading.
2. In Arduino IDE, select **Tools -> Board -> DOIT ESP32 DEVKIT V1**.
3. Select **Tools -> Port** and choose your new CP210x COM port (e.g. COM9).
4. Paste the joystick control code from `Doc.txt` into the IDE.
5. Click **Upload**.
6. **CRITICAL:** When the bottom console says `Connecting......._____`, press and **HOLD** the **"BOOT"** button on the ESP32 board until the upload percentage starts increasing.

---

## 2. Hardware Wiring (Relays & Motors)

### 2.1 The Relay H-Bridge Concept
To reverse the motors, we use an H-Bridge configuration using pairs of relays.
1. **NO (Normally Open):** Bridge ALL of these together and connect to the **Positive (+)** side of your power supply (5V for testing, 24V for final).
2. **NC (Normally Closed):** Bridge ALL of these together and connect to the **Ground (-)** side of your power supply.
3. **COM (Common):** The two wires from your motor go here (One wire to Relay 1 COM, one wire to Relay 2 COM).

### 2.2 Logic Power Connections
1. Use the **LM2596 Buck Converter** to step down your SMPS to exactly **5.0V**.
2. Connect this 5.0V to:
   - ESP32 **VIN** pin
   - 4-Channel Relay **VCC** pin
   - 2-Channel Relay **VCC** pin
3. **COMMON GROUND:** The GND from the SMPS, Buck Converter, ESP32, and all relay modules **MUST** be wired together.

### 2.3 ESP32 Signal Pinouts (Active Low)
*   **Lift UP:** D12
*   **Lift DOWN:** D13
*   **Left FWD:** D14
*   **Left BWD:** D27
*   **Right FWD:** D26
*   **Right BWD:** D25

---

## 3. Web Interface Controls

### 3.1 Lifting
- **LIFT UP / DOWN Buttons:** These are touch-sensitive. The motor will run as long as you hold the button and stop as soon as you lift your finger.

### 3.2 Navigation (Joystick)
- **Virtual Joystick:** Drag the green circle to move the trolley.
- **Directional Logic:**
  - Drag Up: Forward
  - Drag Down: Backward
  - Drag Left/Right: Turning
- **Release:** The joystick will snap back to the center and the trolley will stop immediately.

---

## 4. Troubleshooting
1. **Unresponsive Buttons:** Ensure your phone is connected to the "Trolley_Pro" WiFi.
2. **Silent Relays:** Check the **JD-VCC** jumper on the relay module. It must be plugged in for the relays to click.
3. **Ghosting/Lag:** The code includes a command-buffer that prevents the ESP32 from being flooded with requests. If it lags, try moving the joystick more slowly.