# ESP32-C3 Desktop Companion Monitor

[![Framework](https://img.shields.io/badge/Framework-Arduino_ESP32-blue.svg)](https://espressif.com)
[![Hardware](https://img.shields.io/badge/Hardware-ESP32--C3_Supermini-orange.svg)](https://espressif.com)
[![Enclosure](https://img.shields.io/badge/Enclosure-3D_Printed_Bambulab_P2S-purple.svg)](https://bambulab.com)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

An interactive, zero-flicker desktop companion monitor powered by an **ESP32-C3 Supermini**, an **ST7789 240x240 SPI display**, a **TTP223 capacitive touch sensor**, and a **3D-printed enclosure crafted on a Bambulab P2S**.

Includes 5 customizable pages, zero-flicker differential rendering, Open-Meteo weather integration, Claude Code AI metrics, Matrix digital rain, Star Wars opening crawl credits, Pac-Man arcade action, and an AFK sleeping mascot mode.

---

## 🌟 Key Features

1. **Clock (4 Customizable Watch Faces)**:
   * **Face 0 (Minimal Digital)**: Clean digital layout with Wi-Fi status and weather complication.
   * **Face 1 (Apple/Nike Duo-Color GIANT)**: Stacked size 10 digits with electric blue & coral theme.
   * **Face 2 (Classic Analog Chronograph)**: Zero-flicker 1-pixel differential hand erasing with sub-dials.
   * **Face 3 (Retro Striped)**: Cream & gold stacked numbers with `DD-MM-YYYY` date formatting.
   * **Vector Weather Icons**: Sun ☀️ / Moon 🌙 (day/night API detection), Cloud ☁️, Rain 🌧️, and Storm 🌩️ drawn with GFX primitives.
2. **Dedicated Weather Page**:
   * Powered by **Open-Meteo API** (100% Free, **Zero API key required**).
   * Displays temperature, a large **32x32px vector weather icon**, condition text, relative humidity, and centered API credit.
3. **Claude Code Usage**:
   * Token counter, session quota card, weekly quota card (`WEEKLY`), and status indicator in Anthropic Obsidian dark mode.
4. **Claude AI Companion Mascot**:
   * Animated pixel-art robot mascot with speech bubble (*"Hola Alejo!"*), facial expressions, and touch-petted heart reaction (`<3 PURR! <3`).
5. **Matrix Rain, Star Wars Credits & Arcade Page**:
   * **Mode 0 (Matrix Rain)**: 60fps green digital rain screen refresher.
   * **Mode 1 (Star Wars Opening Crawl)**: Custom Star Wars intro sequence with twinkling starfield, yellow credits crawl, and project backstory.
   * **Mode 2 (Pac-Man Arcade)**: Classic Pac-Man animation with ghost color cycling (Blinky 🔴, Pinky 💖, Inky 💙, Clyde 🧡).
   * **Mode 3 (AFK Sleep Mode)**: Cozy 3D-styled pixel art bedroom with Claude mascot sleeping in a wooden bed with floating `Zzz` animation.
6. **Wi-Fi Captive Portal & AP Web Server**:
   * Hotspot AP `Companion-Setup` on `http://192.168.4.1` with NVS Flash memory storage for easy Wi-Fi configuration via mobile phone or laptop.

---

## 🔌 Wiring Diagram

For detailed wire color mappings and mounting instructions, see **[wiring_diagram.md](wiring_diagram.md)**.

| ST7789 Pin | ESP32-C3 GPIO |
|------------|---------------|
| SCL / SCK   | GPIO 4        |
| SDA / MOSI  | GPIO 6        |
| CS          | GPIO 7        |
| DC          | GPIO 2        |
| RST         | GPIO 10       |
| BLK         | GPIO 3        |
| VCC         | 3V3           |
| GND         | GND           |

| TTP223 Touch Pin | ESP32-C3 GPIO |
|------------------|---------------|
| OUT              | GPIO 5        |
| VCC              | 3V3           |
| GND              | GND           |

---

## 🛠️ Build & Flash Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/Alejandrokbo/ESP32-C3-desktop-companion.git
   cd ESP32-C3-desktop-companion
   ```
2. Create your local secrets file:
   ```bash
   cp src/config.h.example src/config.h
   ```
3. Edit `src/config.h` with your Wi-Fi credentials or latitude/longitude coordinates (or leave empty to configure via Wi-Fi AP setup portal).
4. Build and upload using [PlatformIO](https://platformio.org/):
   ```bash
   pio run -t upload
   ```

---

## 🐍 PC Agent Script

The PC side script streams live Claude Code usage metrics to the ESP32 over USB CDC Serial:

```bash
cd pc-agent
pip install -r requirements.txt
python companion_agent.py COM5
```

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
