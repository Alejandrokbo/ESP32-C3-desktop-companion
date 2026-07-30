#pragma once
#include "../core/Page.h"
#include "../core/PcLink.h"

// Shows CPU/RAM/GPU usage received from the pc-agent Python script over
// USB serial (see PcLink.h). Redraws on a fixed tick since the underlying
// values already arrive throttled from the PC side.
class SystemMonitorPage : public Page {
public:
    explicit SystemMonitorPage(PcLink &link) : _link(link) {}

    const char *name() const override { return "System Monitor"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        tft.fillScreen(ST77XX_BLACK);
        _lastDrawMs = 0; // force immediate redraw
    }

    void update(Adafruit_ST7789 &tft) override {
        uint32_t now = millis();
        if (now - _lastDrawMs < REDRAW_INTERVAL_MS) return;
        _lastDrawMs = now;

        tft.fillScreen(ST77XX_BLACK);

        if (!_link.isOnline()) {
            tft.setTextColor(ST77XX_RED);
            tft.setTextSize(2);
            tft.setCursor(30, 110);
            tft.print("PC offline");
            return;
        }

        _drawBar(tft, "CPU", _link.cpuPercent(), 30, ST77XX_CYAN);
        _drawBar(tft, "RAM", _link.ramPercent(), 100, ST77XX_YELLOW);
        _drawBar(tft, "GPU", _link.gpuPercent(), 170, ST77XX_MAGENTA);
    }

private:
    static constexpr uint32_t REDRAW_INTERVAL_MS = 500;
    static constexpr int BAR_X = 20;
    static constexpr int BAR_W = 200;
    static constexpr int BAR_H = 22;

    void _drawBar(Adafruit_ST7789 &tft, const char *label, float percent, int y, uint16_t color) {
        percent = percent < 0 ? 0 : (percent > 100 ? 100 : percent);

        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.setCursor(BAR_X, y - 20);
        tft.print(label);

        char pctStr[8];
        snprintf(pctStr, sizeof(pctStr), "%3.0f%%", percent);
        tft.setCursor(BAR_X + BAR_W - 50, y - 20);
        tft.print(pctStr);

        tft.drawRect(BAR_X, y, BAR_W, BAR_H, ST77XX_WHITE);
        int fillW = (int)((BAR_W - 2) * (percent / 100.0f));
        tft.fillRect(BAR_X + 1, y + 1, fillW, BAR_H - 2, color);
        if (fillW < BAR_W - 2) {
            tft.fillRect(BAR_X + 1 + fillW, y + 1, (BAR_W - 2) - fillW, BAR_H - 2, ST77XX_BLACK);
        }
    }

    PcLink &_link;
    uint32_t _lastDrawMs = 0;
};
