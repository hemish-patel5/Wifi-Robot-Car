#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "secret.h"

// ===== PIN DEFINITIONS =====
const int PWMA = 12;
const int AIN2 = 14;
const int AIN1 = 27;
const int BIN1 = 26;
const int BIN2 = 25;
const int PWMB = 33;
const int freq = 5000;
const int resolution = 8;

WebServer server(80);

// ===== MOVEMENT FUNCTIONS =====
void stopMotors()
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
}

void moveForward(int speed)
{
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
}

void moveBackward(int speed)
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
}

void left(int speed)
{
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
}

void right(int speed)
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
}

// ===== WEB INTERFACE (TOUCH + WASD) =====
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
<style>
    button { width: 100px; height: 100px; font-size: 20px; margin: 10px; border-radius: 15px; }
</style>
</head>
<body>
    <h1>Robot Control</h1>
    <div>
        <button ontouchstart="fetch('/f')" ontouchend="fetch('/s')">FORWARD</button><br>
        <button ontouchstart="fetch('/l')" ontouchend="fetch('/s')">LEFT</button>
        <button ontouchstart="fetch('/r')" ontouchend="fetch('/s')">RIGHT</button><br>
        <button ontouchstart="fetch('/b')" ontouchend="fetch('/s')">BACKWARD</button>
    </div>

    <script>
        // WASD Keyboard Support
        document.addEventListener('keydown', (e) => {
            if (e.key === 'w') fetch('/f');
            if (e.key === 'a') fetch('/l');
            if (e.key === 'd') fetch('/r');
            if (e.key === 's') fetch('/b');
        });
        document.addEventListener('keyup', (e) => fetch('/s'));
    </script>
</body></html>)rawliteral";

void setup()
{
    Serial.begin(115200);
    ledcAttachPin(PWMA, 0);
    ledcAttachPin(PWMB, 1);
    ledcSetup(0, freq, resolution);
    ledcSetup(1, freq, resolution);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
        delay(500);
    Serial.println(WiFi.localIP());

    server.on("/", []()
              { server.send(200, "text/html", index_html); });
    server.on("/f", []()
              { moveForward(225); server.send(200); });
    server.on("/b", []()
              { moveBackward(225); server.send(200); });
    server.on("/l", []()
              { left(200); server.send(200); });
    server.on("/r", []()
              { right(200); server.send(200); });
    server.on("/s", []()
              { stopMotors(); server.send(200); });
    server.begin();
}

void loop() { server.handleClient(); }