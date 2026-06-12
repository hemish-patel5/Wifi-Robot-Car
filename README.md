# 🤖 Wifi Robot Car

A Wi-Fi controlled robot car built with an ESP32 and a React web app. Control the robot from any browser on your local network — no Bluetooth, no app install required.

> Built by **Hemish** (Software Engineer) & **Harmish** (Electrical Engineer)

---

## Demo

> \_Adding a picture here

---

## Features

- Real-time control via WebSocket over Wi-Fi
- Responsive web controller with D-pad and speed slider
- Hold-to-move buttons — releases automatically on lift
- Safety stop on browser disconnect
- Works from any device on the same network (phone, tablet, laptop)

---

## Project Structure

```text
Wifi-Robot-Car/
|- backend/
|  |- include/
|  |  `- secrets.h          # local only, gitignored
|  |- src/
|  |  `- main.cpp
|  `- platformio.ini
`- frontend/
   |- src/
   |  |- App.jsx
   |  |- index.css
   |  `- main.jsx
   `- package.json
```

## Hardware

| Component       | Details                     |
| --------------- | --------------------------- |
| Microcontroller | ESP32 DevKitC               |
| Motor Driver    | TB6612FNG Dual Motor Driver |
| Motors          | 4x DC Electric Motor        |
| Power           | 4 x AA Batteries            |
| Chassis         | Custom 3D printed casing    |

## Hardware Wiring

These pin mappings match the real firmware in `backend/src/main.cpp`.

| Signal | ESP32 GPIO |
| ------ | ---------- |
| PWMA   | 12         |
| AIN2   | 14         |
| AIN1   | 27         |
| BIN1   | 26         |
| BIN2   | 25         |
| PWMB   | 33         |

Make sure the motor driver and ESP32 share a common ground. Power the motors separately from the ESP32 logic supply.

## Controls

| Button       | Action                      |
| ------------ | --------------------------- |
| ▲            | Forward                     |
| ▼            | Backward                    |
| ◄            | Turn Left                   |
| ►            | Turn Right                  |
| ■            | Stop                        |
| Speed Slider | Adjust motor speed (80–255) |

---

## Roles

| Name    | Role                | Responsibilities                                                        |
| ------- | ------------------- | ----------------------------------------------------------------------- |
| Hemish  | Software Developer  | ESP32 firmware, WebSocket server, React web app                         |
| Harmish | Electrical Engineer | Motor wiring, power setup, motor driver connections, hardware debugging |

---

## Roadmap

- [x] Phase 1 — Assemble chassis and wire motors
- [x] Phase 2 — ESP32 motor control code
- [x] Phase 3 — WebSocket server + web app controller
- [ ] Phase 4 — Joystick control + improved UI
- [ ] Phase 5 — Obstacle avoidance (ultrasonic sensor)
- [ ] Phase 6 — Autonomous navigation

---
