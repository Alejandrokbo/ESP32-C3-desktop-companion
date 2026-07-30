# 🔌 Esquema de Cableado y Colores — Companion Monitor

Guía limpia de conexiones de pines y colores sugeridos de cables Dupont.

---

## 📺 1. Cableado de la Pantalla ST7789 (8 Cables)

* 🔴 **Rojo** ➔ Conectar **VCC** de la pantalla al **3V3** del ESP32-C3
* ⬛ **Negro** ➔ Conectar **GND** de la pantalla al **GND** del ESP32-C3
* 🟡 **Amarillo** ➔ Conectar **SCL / SCK** de la pantalla al **GPIO 4**
* 🟢 **Verde** ➔ Conectar **SDA / MOSI** de la pantalla al **GPIO 6**
* ⚪ **Blanco** ➔ Conectar **RES / RST** de la pantalla al **GPIO 10**
* 🔵 **Azul** ➔ Conectar **DC / RS** de la pantalla al **GPIO 2**
* 🟣 **Morado** ➔ Conectar **CS** de la pantalla al **GPIO 7**
* 🟧 **Naranja** ➔ Conectar **BLK / LED** de la pantalla al **GPIO 3**

---

## 👆 2. Cableado del Sensor Táctil TTP223 (3 Cables)

* 🔴 **Rojo** ➔ Conectar **VCC** del TTP223 al **3V3** del ESP32-C3
* ⬛ **Negro** ➔ Conectar **GND** del TTP223 al **GND** del ESP32-C3
* 🟤 **Marrón** ➔ Conectar **OUT / SIG** del TTP223 al **GPIO 5**

---

## 📋 3. Tabla Compacta de Conexiones

| Componente | Pin Dispositivo | Pin ESP32-C3 | Color Cable |
| :--- | :--- | :--- | :--- |
| **Pantalla** | GND | GND | ⬛ Negro |
| **Pantalla** | VCC | 3V3 | 🔴 Rojo |
| **Pantalla** | SCL / SCK | GPIO 4 | 🟡 Amarillo |
| **Pantalla** | SDA / MOSI | GPIO 6 | 🟢 Verde |
| **Pantalla** | RES / RST | GPIO 10 | ⚪ Blanco |
| **Pantalla** | DC / RS | GPIO 2 | 🔵 Azul |
| **Pantalla** | CS | GPIO 7 | 🟣 Morado |
| **Pantalla** | BLK / LED | GPIO 3 | 🟧 Naranja |
| **Sensor Táctil** | GND | GND | ⬛ Negro |
| **Sensor Táctil** | VCC | 3V3 | 🔴 Rojo |
| **Sensor Táctil** | OUT / SIG | GPIO 5 | 🟤 Marrón |

---

## 💡 Notas Rápidas

1. **VCC (3.3V) y GND**: Como la pantalla y el táctil usan 3.3V y GND, une los 2 cables rojos al pin `3V3` y los 2 cables negros al pin `GND`.
2. **Puerto USB-C**: Con el conector USB-C hacia abajo, `GPIO 4`, `GPIO 6` y `GPIO 5` quedan en el lateral derecho de la placa.
