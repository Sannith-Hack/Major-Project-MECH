/*
 * Wireless Weightlifting Trolley - Trolley Pro
 * Developer: P.Sannith (CSE 3rd Year)
 */

#include <WiFi.h>
#include <WebServer.h>

// Pins
const int liftUpPin = 12; const int liftDownPin = 13;
const int leftFwdPin = 14; const int leftBwdPin = 27;
const int rightFwdPin = 26; const int rightBwdPin = 25;

WebServer server(80);

// NEW RELAY CONTROL FUNCTION (Safest Method)
// Switches between OUTPUT/LOW (ON) and INPUT (OFF) 
// This forces Active-Low relays to stay OFF regardless of ESP32 voltage.
void relayWrite(int pin, bool turnOn) {
  if (turnOn) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Pulls to GND to turn ON
  } else {
    pinMode(pin, INPUT);    // Disconnects pin to turn OFF (Strongest OFF)
  }
}

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
    <p style="margin-top:-15px; color:#aaa; font-size:12px;">Developed by P.Sannith (CSE 3rd Year)</p>
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

// Stop commands
void stopLift() { relayWrite(liftUpPin, false); relayWrite(liftDownPin, false); server.send(200); }
void stopNav() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, false); server.send(200); }

// Navigation logic
void forward() { relayWrite(leftFwdPin, true); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, true); relayWrite(rightBwdPin, false); server.send(200); }
void backward() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, true); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, true); server.send(200); }
void leftPivot() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, true); relayWrite(rightBwdPin, false); server.send(200); }
void rightPivot() { relayWrite(leftFwdPin, true); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, false); server.send(200); }
void leftPlace() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, true); relayWrite(rightFwdPin, true); relayWrite(rightBwdPin, false); server.send(200); }
void rightPlace() { relayWrite(leftFwdPin, true); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, true); server.send(200); }
void bwdLeft() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, false); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, true); server.send(200); }
void bwdRight() { relayWrite(leftFwdPin, false); relayWrite(leftBwdPin, true); relayWrite(rightFwdPin, false); relayWrite(rightBwdPin, false); server.send(200); }

// Lifting logic
void liftUp() { relayWrite(liftUpPin, true); relayWrite(liftDownPin, false); server.send(200); }
void liftDown() { relayWrite(liftUpPin, false); relayWrite(liftDownPin, true); server.send(200); }

void setup() {
  Serial.begin(115200);
  int pins[] = {12, 13, 14, 27, 26, 25};
  for(int p : pins) { pinMode(p, INPUT); } // Set to High-Impedance (OFF)
  
  WiFi.softAP("Trolley_Pro", "12345678");
  server.on("/", handleRoot);
  server.on("/liftup", liftUp); server.on("/liftdown", liftDown); server.on("/stoplift", stopLift);
  server.on("/fwd", forward); server.on("/bwd", backward);
  server.on("/left_pivot", leftPivot); server.on("/right_pivot", rightPivot);
  server.on("/left_place", leftPlace); server.on("/right_place", rightPlace);
  server.on("/bwd_left", bwdLeft); server.on("/bwd_right", bwdRight);
  server.on("/stopnav", stopNav);
  
  server.begin();
  Serial.println("Final High-Impedance Fix Applied.");
}

void loop() { server.handleClient(); }
