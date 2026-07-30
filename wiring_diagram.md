# 🔌 Wiring Diagram & Cable Color Guide — Companion Monitor

Clean pin connection guide and suggested Dupont cable color mappings.

---

## 📺 1. ST7789 Display Wiring (8 Wires)

* 🔴 **Red** ➔ Connect **VCC** on Display to **3V3** on ESP32-C3
* ⬛ **Black** ➔ Connect **GND** on Display to **GND** on ESP32-C3
* 🟡 **Yellow** ➔ Connect **SCL / SCK** on Display to **GPIO 4**
* 🟢 **Green** ➔ Connect **SDA / MOSI** on Display to **GPIO 6**
* ⚪ **White** ➔ Connect **RES / RST** on Display to **GPIO 10**
* 🔵 **Blue** ➔ Connect **DC / RS** on Display to **GPIO 2**
* 🟣 **Purple** ➔ Connect **CS** on Display to **GPIO 7**
* 🟧 **Orange** ➔ Connect **BLK / LED** on Display to **GPIO 3**

---

## 👆 2. TTP223 Touch Sensor Wiring (3 Wires)

* 🔴 **Red** ➔ Connect **VCC** on TTP223 to **3V3** on ESP32-C3
* ⬛ **Black** ➔ Connect **GND** on TTP223 to **GND** on ESP32-C3
* 🟤 **Brown** ➔ Connect **OUT / SIG** on TTP223 to **GPIO 5**

---

## 📋 3. Pinout Reference Table

| Component | Device Pin | ESP32-C3 Pin | Cable Color |
| :--- | :--- | :--- | :--- |
| **Display** | GND | GND | ⬛ Black |
| **Display** | VCC | 3V3 | 🔴 Red |
| **Display** | SCL / SCK | GPIO 4 | 🟡 Yellow |
| **Display** | SDA / MOSI | GPIO 6 | 🟢 Green |
| **Display** | RES / RST | GPIO 10 | ⚪ White |
| **Display** | DC / RS | GPIO 2 | 🔵 Blue |
| **Display** | CS | GPIO 7 | 🟣 Purple |
| **Display** | BLK / LED | GPIO 3 | 🟧 Orange |
| **Touch Sensor** | GND | GND | ⬛ Black |
| **Touch Sensor** | VCC | 3V3 | 🔴 Red |
| **Touch Sensor** | OUT / SIG | GPIO 5 | 🟤 Brown |

---

## 💡 Quick Tips

1. **Power Lines (3.3V & GND)**: Since both the display and touch sensor require 3.3V and GND, bridge the two red wires to `3V3` and the two black wires to `GND`.
2. **Capacitive Touch Mounting**: The TTP223 touch sensor can be mounted inside a 3D-printed or plastic enclosure using thin double-sided tape without losing touch sensitivity.
