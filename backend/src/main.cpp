#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include "SetupPage.h"

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
Preferences preferences;

const char *setupApSsid = "RobotCar-Setup";
const char *setupApPassword = "password";
const char *mdnsName = "robotcar";
const unsigned long wifiConnectTimeoutMs = 15000;
const unsigned long motorCommandTimeoutMs = 350;

bool setupMode = false;
bool motorsActive = false;
unsigned long lastMotorCommandAt = 0;

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

String jsonEscape(const String &value)
{
    String escaped = value;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.replace("\n", "\\n");
    escaped.replace("\r", "\\r");
    return escaped;
}

void handleRoot()
{
    addCorsHeaders();

    if (setupMode)
    {
        server.send(200, "text/html", buildSetupPage(WiFi.softAPIP().toString()));
        return;
    }

    String status = "RobotCar connected to " + htmlEscape(WiFi.SSID()) + "\n";
    status += "IP: " + WiFi.localIP().toString() + "\n";
    server.send(200, "text/plain", status);
}

void handleWifiSave()
{
    if (!server.hasArg("ssid"))
    {
        sendText(400, "Missing ssid");
        return;
    }

    String ssid = server.arg("ssid");
    String password = server.arg("password");
    ssid.trim();

    if (ssid.length() == 0)
    {
        sendText(400, "SSID cannot be empty");
        return;
    }

    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();

    addCorsHeaders();
    server.send(200, "text/html", "<!doctype html><html><body><h1>Saved</h1><p>Robot is restarting and will try to join your WiFi.</p></body></html>");
    delay(1000);
    ESP.restart();
}

void handleWifiReset()
{
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();

    sendText(200, "WiFi credentials cleared. Restarting.");
    delay(1000);
    ESP.restart();
}

void handleWifiScan()
{
    addCorsHeaders();
    int networkCount = WiFi.scanNetworks();
    String json = "[";

    for (int i = 0; i < networkCount; i++)
    {
        if (i > 0)
        {
            json += ",";
        }

        json += "{\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
    }

    json += "]";
    server.send(200, "application/json", json);
}

bool connectToSavedWifi()
{
    preferences.begin("wifi", true);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    preferences.end();

    if (ssid.length() == 0)
    {
        Serial.println("No saved WiFi credentials.");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    unsigned long startedAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startedAt < wifiConnectTimeoutMs)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected. IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    }

    Serial.println("Could not connect to saved WiFi.");
    WiFi.disconnect(true);
    return false;
}

void startMdns()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    if (MDNS.begin(mdnsName))
    {
        MDNS.addService("http", "tcp", 80);
        Serial.print("mDNS started: http://");
        Serial.print(mdnsName);
        Serial.println(".local");
    }
    else
    {
        Serial.println("mDNS failed to start.");
    }
}

void startSetupAccessPoint()
{
    setupMode = true;
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(setupApSsid, setupApPassword);

    Serial.print("Setup AP started: ");
    Serial.println(setupApSsid);
    Serial.print("Setup AP IP address: ");
    Serial.println(WiFi.softAPIP());
}

void setupRoutes()
{
    server.on("/", HTTP_GET, handleRoot);
    server.on("/wifi", HTTP_POST, handleWifiSave);
    server.on("/wifi/reset", HTTP_POST, handleWifiReset);
    server.on("/scan", HTTP_GET, handleWifiScan);
    server.onNotFound([]()
                      {
                          if (server.method() == HTTP_OPTIONS)
                          {
                              sendText(204, "");
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
    stopMotors();

    if (connectToSavedWifi())
    {
        startMdns();
    }
    else
    {
        startSetupAccessPoint();
    }

    setupRoutes();
    server.begin();
}

void loop()
{
    server.handleClient();
    enforceMotorTimeout();
}
