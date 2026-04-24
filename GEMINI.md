# Wireless Weightlifting Trolley - Project Overview

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

## Current Control Code
```cpp
#include <WiFi.h>
#include <WebServer.h>

const int liftUpPin = 12; const int liftDownPin = 13;
const int leftFwdPin = 14; const int leftBwdPin = 27;
const int rightFwdPin = 26; const int rightBwdPin = 25;

WebServer server(80);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html><html><head><title>Trolley Pro</title>
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
<style>
  body { text-align:center; font-family:sans-serif; background-color:#121212; color:white; margin:0; overflow:hidden; user-select:none; -webkit-user-select:none; }
  .container { display:flex; flex-direction:column; align-items:center; justify-content:center; height:100vh; }
  .lift-box { margin-bottom: 20px; width: 100%; }
  .btn { width:40%; height:80px; font-size:18px; margin:10px; border-radius:15px; border:none; color:white; font-weight:bold; background:#007bff; touch-action:none; }
  .btn:active { background:#004a99; }
  #joystick-container { position:relative; width:200px; height:200px; background:rgba(255,255,255,0.1); border-radius:50%; margin:20px; border: 2px solid #444; touch-action:none; }
  #joystick-stick { position:absolute; width:80px; height:80px; background:#28a745; border-radius:50%; top:60px; left:60px; box-shadow: 0 0 15px rgba(0,0,0,0.5); }
</style></head>
<body>
  <div class="container">
    <h2>TROLLEY PRO</h2>
    <div class="lift-box">
      <button class="btn" ontouchstart="fetch('/liftup')" ontouchend="fetch('/stoplift')" onmousedown="fetch('/liftup')" onmouseup="fetch('/stoplift')">LIFT UP</button>
      <button class="btn" ontouchstart="fetch('/liftdown')" ontouchend="fetch('/stoplift')" onmousedown="fetch('/liftdown')" onmouseup="fetch('/stoplift')">LIFT DOWN</button>
    </div>
    <div id="joystick-container"><div id="joystick-stick"></div></div>
    <p id="status">READY</p>
  </div>
  <script>
    const stick = document.getElementById('joystick-stick');
    const container = document.getElementById('joystick-container');
    let lastCmd = "";
    container.addEventListener('touchstart', handleTouch);
    container.addEventListener('touchmove', handleTouch);
    container.addEventListener('touchend', () => { stick.style.left = '60px'; stick.style.top = '60px'; sendCmd("/stopnav"); });
    function handleTouch(e) {
      e.preventDefault(); const rect = container.getBoundingClientRect(); const touch = e.touches[0];
      const x = touch.clientX - rect.left - 100; const y = touch.clientY - rect.top - 100;
      const dist = Math.min(Math.sqrt(x*x + y*y), 60); const angle = Math.atan2(y, x);
      stick.style.left = (Math.cos(angle) * dist + 60) + 'px'; stick.style.top = (Math.sin(angle) * dist + 60) + 'px';
      if (dist > 25) {
        let cmd = ""; const deg = angle * 180 / Math.PI;
        if (deg >= -22.5 && deg < 22.5) cmd = "/right_pivot";
        else if (deg >= -67.5 && deg < -22.5) cmd = "/right_place";
        else if (deg >= -112.5 && deg < -67.5) cmd = "/fwd";
        else if (deg >= -157.5 && deg < -112.5) cmd = "/left_place";
        else if (deg >= 157.5 || deg < -157.5) cmd = "/left_pivot";
        else if (deg >= 112.5 && deg < 157.5) cmd = "/bwd_left";
        else if (deg >= 67.5 && deg < 112.5) cmd = "/bwd";
        else if (deg >= 22.5 && deg < 67.5) cmd = "/bwd_right";
        sendCmd(cmd);
      } else { sendCmd("/stopnav"); }
    }
    function sendCmd(cmd) { if (lastCmd !== cmd) { fetch(cmd); lastCmd = cmd; document.getElementById('status').innerText = "MOVING: " + cmd.replace("/","").toUpperCase().replace("_"," "); } }
  </script>
</body></html>
)rawliteral";

void handleRoot() { server.send(200, "text/html", htmlPage); }
void stopLift() { digitalWrite(liftUpPin, HIGH); digitalWrite(liftDownPin, HIGH); server.send(200); }
void stopNav() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void forward() { digitalWrite(leftFwdPin, LOW); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, LOW); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void backward() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, LOW); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, LOW); server.send(200); }
void leftPivot() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, LOW); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void rightPivot() { digitalWrite(leftFwdPin, LOW); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void leftPlace() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, LOW); digitalWrite(rightFwdPin, LOW); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void rightPlace() { digitalWrite(leftFwdPin, LOW); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, LOW); server.send(200); }
void bwdLeft() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, HIGH); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, LOW); server.send(200); }
void bwdRight() { digitalWrite(leftFwdPin, HIGH); digitalWrite(leftBwdPin, LOW); digitalWrite(rightFwdPin, HIGH); digitalWrite(rightBwdPin, HIGH); server.send(200); }
void liftUp() { digitalWrite(liftUpPin, LOW); digitalWrite(liftDownPin, HIGH); server.send(200); }
void liftDown() { digitalWrite(liftUpPin, HIGH); digitalWrite(liftDownPin, LOW); server.send(200); }

void setup() {
  int pins[] = {12, 13, 14, 27, 26, 25};
  for(int p : pins) { pinMode(p, OUTPUT); digitalWrite(p, HIGH); }
  WiFi.softAP("Trolley_Pro", "12345678");
  server.on("/", handleRoot);
  server.on("/liftup", liftUp); server.on("/liftdown", liftDown); server.on("/stoplift", stopLift);
  server.on("/fwd", forward); server.on("/bwd", backward);
  server.on("/left_pivot", leftPivot); server.on("/right_pivot", rightPivot);
  server.on("/left_place", leftPlace); server.on("/right_place", rightPlace);
  server.on("/bwd_left", bwdLeft); server.on("/bwd_right", bwdRight);
  server.on("/stopnav", stopNav);
  server.begin();
}
void loop() { server.handleClient(); }
```

## Known Troubleshooting & Fixes
*   **Upload Errors (Semaphore Timeout / Port missing):** Requires the **Silicon Labs CP210x USB to UART Bridge** driver. Do not select Bluetooth COM ports.
*   **Connecting... Error during upload:** Press and hold the **"BOOT"** button on the ESP32 when the IDE says `Connecting......._____`.
*   **Silent Relays:** If a relay doesn't click, verify the `JD-VCC` to `VCC` jumper is installed, and High/Low trigger (if applicable) is set to `L` (Low).

## Assembly Checklist
1. [x] Install CP210x Drivers and test ESP32 code upload via USB.
2. [ ] Calibrate Buck Converter to exactly 5.0V.
3. [ ] Connect common grounds across all modules.
4. [ ] Wire H-Bridge (NO to Power, NC to GND, COM to Motors).
5. [ ] Connect ESP32 GPIOs to Relay IN pins.
6. [ ] Upload Code (holding BOOT button) and test WiFi connection (`192.168.4.1`).
