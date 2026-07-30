# ESP32-C3 Desktop Companion Monitor

ESP32-C3 Supermini + ST7789 240x240 SPI display + TTP223 capacitive touch module.

An interactive desktop companion with 4 pages, zero-flicker differential rendering, Open-Meteo weather integration, and Claude Code AI companion mascot.

---

## 🚀 Features

1. **Reloj (4 Esferas Intercambiables)**:
   * **Esfera 0**: Digital minimalista con estado de Wi-Fi y clima.
   * **Esfera 1 (Apple/Nike Duo-Color GIGANTE)**: Números apilados de tamaño 10 con badge del clima.
   * **Esfera 2 (Cronógrafo Analógico)**: Manecillas analógicas sin parpadeo y sub-diales traducidos al español.
   * **Esfera 3 (Retro Apilado)**: Números crema/dorado con fecha en formato `dd-mm-YYYY`.
   * **Iconos Vectoriales del Clima**: Sol ☀️, Nube ☁️, Lluvia 🌧️ y Tormenta 🌩️ en todas las esferas.
2. **El Tiempo Detallado**:
   * API pública gratuita **Open-Meteo** (0 claves de API requeridas).
   * Muestra temperatura en verde eléctrico, **icono vectorial gigante de 32x32px**, condición en español y humedad.
3. **Claude Code Usage**:
   * Tokens consumidos, tarjetas de cuota de Sesión y Semanal con estética Anthropic Obsidian.
4. **Mascota Claude**:
   * Robot pixel-art salmón/terracota con animaciones, globo de texto personal (*"¡Hola Alejo!"*) y reacción al acariciar (*Long Press*).
5. **Wi-Fi Captive Portal**:
   * Red AP Hotspot `Companion-Setup` en `http://192.168.4.1` con memoria NVS no volátil para configurar el Wi-Fi desde el móvil.

---

## 🔌 Cableado (Wiring)

| ST7789 Pin | ESP32-C3 GPIO |
|------------|---------------|
| SCL/SCK    | 4             |
| SDA/MOSI   | 6             |
| CS         | 7             |
| DC         | 2             |
| RST        | 10            |
| BLK        | 3             |
| VCC        | 3V3           |
| GND        | GND           |

| TTP223 Pin | ESP32-C3 GPIO |
|------------|---------------|
| OUT        | 5             |
| VCC        | 3V3           |
| GND        | GND           |

---

## 🛠️ Configuración e Instalación

1. Clona este repositorio:
   ```bash
   git clone https://github.com/Alejandrokbo/ESP32-C3-desktop-companion.git
   cd ESP32-C3-desktop-companion
   ```
2. Copia el plantilla de configuración:
   ```bash
   cp src/config.h.example src/config.h
   ```
3. Rellena tus credenciales Wi-Fi o coordenadas en `src/config.h` (o déjalo vacio para usar el Portal Cautivo Wi-Fi).
4. Compila y flashea con [PlatformIO](https://platformio.org/):
   ```bash
   pio run -t upload
   ```

---

## 🐍 Agente de PC (Python)

El agente de PC envía las métricas de uso de Claude Code al ESP32 a través del puerto serie USB:

```bash
cd pc-agent
pip install -r requirements.txt
python companion_agent.py COM5
```

---

## 📄 Licencia

MIT License © Alejandrokbo
