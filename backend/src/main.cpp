#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include "UltrasonicSensor.h"

// ===== PIN DEFINITIONS =====
const int PWMA = 12;
const int AIN2 = 14;
const int AIN1 = 27;
const int BIN1 = 26;
const int BIN2 = 25;
const int PWMB = 33;
const int ECHO_PIN = 22;
const int TRIG_PIN = 23;
const int freq = 5000;
const int resolution = 8;

WebServer server(80);
DNSServer dnsServer;

const char *controlApSsid = "RobotCar";
const char *controlApPassword = "password";
const char *controlHostName = "robot.car";
const byte dnsPort = 53;
const unsigned long motorCommandTimeoutMs = 350;

bool motorsActive = false;
unsigned long lastMotorCommandAt = 0;
UltrasonicSensor ultrasonicSensor(TRIG_PIN, ECHO_PIN);

// ===== MOVEMENT FUNCTIONS =====
void markMotorCommand()
{
    motorsActive = true;
    lastMotorCommandAt = millis();
}

void stopMotors()
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    motorsActive = false;
}

void moveForward(int speed)
{
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
    markMotorCommand();
}

void moveBackward(int speed)
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
    markMotorCommand();
}

void left(int speed)
{
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
    markMotorCommand();
}

void right(int speed)
{
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
    markMotorCommand();
}

void enforceMotorTimeout()
{
    if (motorsActive && millis() - lastMotorCommandAt > motorCommandTimeoutMs)
    {
        stopMotors();
        Serial.println("Motor command timed out; stopping.");
    }
}

void addCorsHeaders()
{
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void sendText(int statusCode, const String &message)
{
    addCorsHeaders();
    server.send(statusCode, "text/plain", message);
}

String getContentType(const String &path)
{
    if (path.endsWith(".html"))
    {
        return "text/html";
    }

    if (path.endsWith(".css"))
    {
        return "text/css";
    }

    if (path.endsWith(".js"))
    {
        return "application/javascript";
    }

    if (path.endsWith(".svg"))
    {
        return "image/svg+xml";
    }

    if (path.endsWith(".json"))
    {
        return "application/json";
    }

    if (path.endsWith(".ico"))
    {
        return "image/x-icon";
    }

    return "text/plain";
}

bool serveStaticFile(String path)
{
    if (path.endsWith("/"))
    {
        path += "index.html";
    }

    if (!SPIFFS.exists(path))
    {
        return false;
    }

    File file = SPIFFS.open(path, "r");

    if (!file)
    {
        return false;
    }

    addCorsHeaders();
    server.streamFile(file, getContentType(path));
    file.close();
    return true;
}

String htmlEscape(const String &value)
{
    String escaped = value;
    escaped.replace("&", "&amp;");
    escaped.replace("<", "&lt;");
    escaped.replace(">", "&gt;");
    escaped.replace("\"", "&quot;");
    escaped.replace("'", "&#39;");
    return escaped;
}

void handleRoot()
{
    if (serveStaticFile("/index.html"))
    {
        return;
    }

    String status = "RobotCar access point: " + htmlEscape(String(controlApSsid)) + "\n";
    status += "IP: " + WiFi.softAPIP().toString() + "\n";
    status += "Control app: http://" + String(controlHostName) + "\n";
    sendText(200, status);
}

void startControlAccessPoint()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPsetHostname(controlHostName);
    bool started = WiFi.softAP(controlApSsid, controlApPassword);
    dnsServer.start(dnsPort, controlHostName, WiFi.softAPIP());

    Serial.print("Control AP ");
    Serial.println(started ? "started." : "failed to start.");
    Serial.print("SSID: ");
    Serial.println(controlApSsid);
    Serial.print("Password: ");
    Serial.println(controlApPassword);
    Serial.print("IP control app: http://");
    Serial.println(WiFi.softAPIP());
    Serial.print("Named control app: http://");
    Serial.println(controlHostName);
}

void setupRoutes()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/status", HTTP_GET, []()
              {
                  String status = "RobotCar access point: " + htmlEscape(String(controlApSsid)) + "\n";
                  status += "IP: " + WiFi.softAPIP().toString() + "\n";
                  status += "Control app: http://" + String(controlHostName) + "\n";
                  sendText(200, status); });
    server.onNotFound([]()
                      {
                          if (server.method() == HTTP_OPTIONS)
                          {
                              sendText(204, "");
                              return;
                          }

                          if (serveStaticFile(server.uri()))
                          {
                              return;
                          }

                          sendText(404, "Not found"); });

    server.on("/f", []()
              {
                moveForward(225);
                sendText(200, "OK"); });
    server.on("/b", []()
              {
                  moveBackward(225);
                  sendText(200, "OK"); });
    server.on("/l", []()
              {
                  left(200);
                  sendText(200, "OK"); });
    server.on("/r", []()
              {
                  right(200);
                  sendText(200, "OK"); });
    server.on("/s", []()
              {
                  stopMotors();
                  sendText(200, "OK"); });
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
    ultrasonicSensor.begin();
    stopMotors();

    if (SPIFFS.begin(true))
    {
        Serial.println("SPIFFS mounted. Frontend files are available.");
    }
    else
    {
        Serial.println("SPIFFS mount failed. Frontend files will not be served.");
    }

    startControlAccessPoint();

    setupRoutes();
    server.begin();
    Serial.println("HTTP server started on port 80.");
}

void loop()
{
    dnsServer.processNextRequest();
    server.handleClient();
    enforceMotorTimeout();
    ultrasonicSensor.printProximityScore();
}
