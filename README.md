# 🤖 RoverLink

A Wi-Fi controlled robot car built with an ESP32 and a React web app. Control the robot from any browser on your local network — no Bluetooth, no app install required.

> Built by **Pat** (Software) & **Harmu** (Electrical Engineering)

---

## Demo

> _Add a GIF or photo of the robot here_

---

## Features

- Real-time control via WebSocket over Wi-Fi
- Responsive web controller with D-pad and speed slider
- Hold-to-move buttons — releases automatically on lift
- Safety stop on browser disconnect
- Works from any device on the same network (phone, tablet, laptop)

---

## System Architecture

```
Browser (React Web App)
        │
        │  WebSocket  ws://robot-ip:81
        │
ESP32 WebSocket Server
        │
TB6612FNG Motor Driver
        │
DC Motors (x4)
```

---

## Hardware

| Component | Details |
|-----------|---------|
| Microcontroller | ESP32 DevKitC |
| Motor Driver | TB6612FNG Dual Motor Driver |
| Motors | 4x DC Electric Motor |
| Power | 2x 18650 Li-ion or 2S LiPo + Step-Down Module |
| Chassis | Custom 3D printed casing |

### Wiring

| TB6612FNG Pin | ESP32 GPIO |
|---------------|-----------|
| PWMA | 14 |
| AIN1 | 27 |
| AIN2 | 26 |
| PWMB | 12 |
| BIN1 | 25 |
| BIN2 | 33 |
| STBY | 32 |
| VCC | 3.3V |
| GND | GND |
| VM | Battery + |

> ⚠️ Motors require separate power from the ESP32. Always connect GND together.

---

## Project Structure

```
RoverLink/
├── firmware/          # ESP32 C++ code (PlatformIO)
│   ├── src/
│   │   └── main.cpp
│   └── platformio.ini
└── web-app/           # React frontend
    ├── src/
    │   └── App.jsx
    └── package.json
```

---

## Getting Started

### Prerequisites

- [VS Code](https://code.visualstudio.com/) + [PlatformIO](https://platformio.org/) extension
- [Node.js](https://nodejs.org/) (v18+)
- ESP32 connected via USB

---

### 1. Flash the ESP32

```bash
cd firmware
```

Open `src/main.cpp` and update your Wi-Fi credentials:

```cpp
const char* SSID     = "YOUR_WIFI_NAME";
const char* PASSWORD = "YOUR_WIFI_PASSWORD";
```

Then upload via PlatformIO:

```
PlatformIO → Upload
```

Open the **Serial Monitor** and note the IP address printed, e.g.:
```
Connected! IP: 192.168.1.105
```

---

### 2. Run the Web App

```bash
cd web-app
npm install
npm run dev
```

Open `src/App.jsx` and set the ESP32's IP:

```jsx
const ESP32_IP = "192.168.1.105"; // 👈 Replace with your IP
```

Open the app in your browser. Make sure your device is on the **same Wi-Fi network** as the ESP32.

---

## Controls

| Button | Action |
|--------|--------|
| ▲ | Forward |
| ▼ | Backward |
| ◄ | Turn Left |
| ► | Turn Right |
| ■ | Stop |
| Speed Slider | Adjust motor speed (80–255) |

---

## Roles

| Name | Role | Responsibilities |
|------|------|-----------------|
| Pat | Software Developer | ESP32 firmware, WebSocket server, React web app |
| Harmu | Electrical Engineer | Motor wiring, power setup, motor driver connections, hardware debugging |

---

## Roadmap

- [x] Phase 1 — Assemble chassis and wire motors
- [x] Phase 2 — ESP32 motor control code
- [x] Phase 3 — WebSocket server + web app controller
- [ ] Phase 4 — Joystick control + improved UI
- [ ] Phase 5 — Obstacle avoidance (ultrasonic sensor)
- [ ] Phase 6 — Autonomous navigation

---

## License

MIT License — feel free to use and build on this project.
