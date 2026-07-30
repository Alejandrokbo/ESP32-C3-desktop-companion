#include "DisplayManager.h"

void DisplayManager::begin() {
    // Remap the FSPI peripheral onto our chosen pins (MISO unused -> -1).
    _spi.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

    _tft.init(240, 240);
    _tft.setRotation(2); // Rotate 180 degrees so screen is right side up
    _tft.fillScreen(ST77XX_BLACK);

    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RES_BITS);
    ledcAttachPin(TFT_BLK, BL_PWM_CHANNEL);
    setBacklight(255);
}

void DisplayManager::setBacklight(uint8_t brightness) {
    ledcWrite(BL_PWM_CHANNEL, brightness);
}
