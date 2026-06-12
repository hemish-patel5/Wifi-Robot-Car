#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

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

void setup()
{
    Serial.begin(115200);
    ledcSetup(0, freq, resolution);
    ledcSetup(1, freq, resolution);
    ledcAttachPin(PWMA, 0);
    ledcAttachPin(PWMB, 1);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    WiFi.softAP("RobotCar", "password");
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: " + IP.toString());

    server.on("/f", []()
              {
                server.sendHeader("Access-Control-Allow-Origin", "*");
                moveForward(225);
                server.send(200, "text/plain", "OK"); });
    server.on("/b", []()
              {
                  server.sendHeader("Access-Control-Allow-Origin", "*");
                  moveBackward(225);
                  server.send(200, "text/plain", "OK"); });
    server.on("/l", []()
              {
                  server.sendHeader("Access-Control-Allow-Origin", "*");
                  left(200);
                  server.send(200, "text/plain", "OK"); });
    server.on("/r", []()
              {
                  server.sendHeader("Access-Control-Allow-Origin", "*");
                  right(200);
                  server.send(200, "text/plain", "OK"); });
    server.on("/s", []()
              {
                  server.sendHeader("Access-Control-Allow-Origin", "*");
                  stopMotors();
                  server.send(200, "text/plain", "OK"); });
    server.begin();
}

void loop() { server.handleClient(); }