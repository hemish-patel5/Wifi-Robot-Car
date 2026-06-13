#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor
{
public:
    UltrasonicSensor(int trigPin, int echoPin, unsigned long readIntervalMs = 250, unsigned long timeoutUs = 30000)
        : trigPin(trigPin),
          echoPin(echoPin),
          readIntervalMs(readIntervalMs),
          timeoutUs(timeoutUs),
          lastReadAt(0)
    {
    }

    void begin()
    {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
        digitalWrite(trigPin, LOW);
    }

    float readDistanceCm()
    {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        unsigned long duration = pulseIn(echoPin, HIGH, timeoutUs);

        if (duration == 0)
        {
            return -1.0;
        }

        return duration * 0.0343 / 2.0;
    }

    int getProximityScore()
    {
        return distanceToProximityScore(readDistanceCm());
    }

    void printProximityScore()
    {
        if (millis() - lastReadAt < readIntervalMs)
        {
            return;
        }

        lastReadAt = millis();
        Serial.println(getProximityScore());
    }

private:
    int trigPin;
    int echoPin;
    unsigned long readIntervalMs;
    unsigned long timeoutUs;
    unsigned long lastReadAt;

    int distanceToProximityScore(float distanceCm)
    {
        if (distanceCm < 0 || distanceCm >= 100.0)
        {
            return 0;
        }

        if (distanceCm <= 5.0)
        {
            return 100;
        }

        return map((long)distanceCm, 5, 100, 100, 0);
    }
};

#endif
