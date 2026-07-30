#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include "Pins.h"

// Thin wrapper around Adafruit_ST7789 that owns the remapped SPI bus and the
// backlight PWM channel. Pages just call DisplayManager::tft() to draw.
class DisplayManager {
public:
    void begin();

    Adafruit_ST7789 &tft() { return _tft; }

    // 0-255. Uses ledc PWM on TFT_BLK so you can dim it instead of a hard on/off.
    void setBacklight(uint8_t brightness);

private:
    SPIClass _spi{FSPI};
    Adafruit_ST7789 _tft{&_spi, TFT_CS, TFT_DC, TFT_RST};

    static constexpr uint8_t BL_PWM_CHANNEL = 0;
    static constexpr uint32_t BL_PWM_FREQ = 5000;
    static constexpr uint8_t BL_PWM_RES_BITS = 8;
};
