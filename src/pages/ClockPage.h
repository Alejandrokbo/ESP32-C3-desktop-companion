#pragma once
#include "../core/Page.h"
#include "../core/NetManager.h"
#include "../config.h"
#include <time.h>
#include <math.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Clock page with 4 Watch Faces and integrated Weather complications + Vector Icons.
// Long-press cycles between Watch Face styles with zero flicker.
class ClockPage : public Page {
public:
    const char *name() const override { return "Reloj"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        tft.fillScreen(ST77XX_BLACK);
        _lastDrawSec = -1;
        _lastDrawMin = -1;
        _lastFace = -1;
        _prevSx = _prevSy = -1;
        _prevMx = _prevMy = -1;
        _prevHx = _prevHy = -1;
        _triedNtpSync = false;
        _maybeFetchWeather(true);
    }

    void onLongPress(Adafruit_ST7789 &tft) override {
        _watchFace = (_watchFace + 1) % 4; // Cycle watch faces: 0, 1, 2, 3
        tft.fillScreen(ST77XX_BLACK);
        _lastDrawSec = -1;
        _lastDrawMin = -1;
        _prevSx = _prevSy = -1;
        _prevMx = _prevMy = -1;
        _prevHx = _prevHy = -1;
    }

    void update(Adafruit_ST7789 &tft) override {
        if (!_triedNtpSync && NetManager::isConnected()) {
            configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
            _triedNtpSync = true;
        }

        _maybeFetchWeather(false);

        time_t now = time(nullptr);
        struct tm tmNow;
        localtime_r(&now, &tmNow);

        if (tmNow.tm_sec == _lastDrawSec && _watchFace == _lastFace) return;
        _lastDrawSec = tmNow.tm_sec;
        _lastFace = _watchFace;

        switch (_watchFace) {
            case 0: _drawStandardDigital(tft, tmNow); break;
            case 1: _drawGiantBoldDuoColor(tft, tmNow); break;
            case 2: _drawAnalogChronograph(tft, tmNow); break;
            case 3: _drawRetroStriped(tft, tmNow); break;
        }
    }

private:
    static constexpr uint32_t WEATHER_POLL_MS = 600000; // 10 minutes

    int _lastDrawSec = -1;
    int _lastDrawMin = -1;
    int _lastFace = -1;
    int _watchFace = 0;
    bool _triedNtpSync = false;

    // Previous Analog Hand Coordinates
    int _prevSx = -1, _prevSy = -1;
    int _prevMx = -1, _prevMy = -1;
    int _prevHx = -1, _prevHy = -1;

    // Weather Complication State
    uint32_t _lastWeatherFetchMs = 0;
    bool _hasWeather = false;
    float _weatherTemp = 0.0f;
    int _weatherCode = 0;
    String _weatherCond;

    // Palette
    static constexpr uint16_t COLOR_BLUE  = 0x04FF; // Electric Blue (#00A5FF)
    static constexpr uint16_t COLOR_CORAL = 0xF980; // Vibrant Coral Red (#FF3B30)
    static constexpr uint16_t COLOR_GOLD  = 0xFBE0; // Warm Gold / Amber
    static constexpr uint16_t COLOR_CREAM = 0xF77D; // Off-white cream
    static constexpr uint16_t COLOR_GREEN = 0x07E0; // Electric Green

    // Spanish Localization Arrays
    const char *DAYS_ES[7]   = {"DOM", "LUN", "MAR", "MIE", "JUE", "VIE", "SAB"};
    const char *MONTHS_ES[12] = {"ENE", "FEB", "MAR", "ABR", "MAY", "JUN", "JUL", "AGO", "SEP", "OCT", "NOV", "DIC"};

    void _maybeFetchWeather(bool force) {
        if (!NetManager::isConnected()) return;
        uint32_t now = millis();
        if (!force && _lastWeatherFetchMs != 0 && (now - _lastWeatherFetchMs) < WEATHER_POLL_MS) {
            return;
        }
        _lastWeatherFetchMs = now;

        HTTPClient http;
        String url = String("http://api.open-meteo.com/v1/forecast?latitude=") +
                      OWM_LAT + "&longitude=" + OWM_LON +
                      "&current=temperature_2m,weather_code";

        http.setTimeout(10000);
        http.setUserAgent("Mozilla/5.0 (ESP32-C3)");
        http.begin(url);
        int code = http.GET();

        if (code == 200) {
            JsonDocument doc;
            if (!deserializeJson(doc, http.getString())) {
                _weatherTemp = doc["current"]["temperature_2m"].as<float>();
                _weatherCode = doc["current"]["weather_code"].as<int>();

                if (_weatherCode == 0) _weatherCond = "Despejado";
                else if (_weatherCode >= 1 && _weatherCode <= 3) _weatherCond = "Nublado";
                else if (_weatherCode == 45 || _weatherCode == 48) _weatherCond = "Niebla";
                else if (_weatherCode >= 51 && _weatherCode <= 67) _weatherCond = "Lluvia";
                else if (_weatherCode >= 71 && _weatherCode <= 77) _weatherCond = "Nieve";
                else if (_weatherCode >= 80 && _weatherCode <= 82) _weatherCond = "Chubascos";
                else if (_weatherCode >= 95) _weatherCond = "Tormenta";
                else _weatherCond = "Templado";

                _hasWeather = true;
            }
        }
        http.end();
    }

    void _drawWeatherIcon(Adafruit_ST7789 &tft, int x, int y) {
        if (!_hasWeather) return;

        if (_weatherCode == 0) {
            // Sun Icon (Yellow circle + 4 rays)
            tft.fillCircle(x + 6, y + 6, 4, ST77XX_YELLOW);
            tft.drawLine(x + 6, y, x + 6, y + 2, ST77XX_YELLOW);
            tft.drawLine(x + 6, y + 10, x + 6, y + 12, ST77XX_YELLOW);
            tft.drawLine(x, y + 6, x + 2, y + 6, ST77XX_YELLOW);
            tft.drawLine(x + 10, y + 6, x + 12, y + 6, ST77XX_YELLOW);
        } else if (_weatherCode >= 51 && _weatherCode <= 67) {
            // Rain Icon (Cloud + Rain drops)
            tft.fillCircle(x + 4, y + 5, 3, 0xD67A);
            tft.fillCircle(x + 8, y + 4, 4, 0xD67A);
            tft.fillRect(x + 2, y + 6, 9, 3, 0xD67A);
            tft.drawLine(x + 4, y + 10, x + 2, y + 12, 0x07FF);
            tft.drawLine(x + 8, y + 10, x + 6, y + 12, 0x07FF);
        } else if (_weatherCode >= 95) {
            // Storm Icon (Cloud + Lightning)
            tft.fillCircle(x + 4, y + 4, 3, 0xD67A);
            tft.fillCircle(x + 8, y + 3, 4, 0xD67A);
            tft.fillRect(x + 2, y + 5, 9, 3, 0xD67A);
            tft.drawLine(x + 7, y + 8, x + 4, y + 11, ST77XX_YELLOW);
            tft.drawLine(x + 4, y + 11, x + 6, y + 13, ST77XX_YELLOW);
        } else {
            // Cloud Icon (Nublado)
            tft.fillCircle(x + 4, y + 5, 3, 0xD67A);
            tft.fillCircle(x + 8, y + 4, 4, 0xD67A);
            tft.fillRect(x + 2, y + 6, 9, 4, 0xD67A);
        }
    }

    String _getWeatherSummaryStr() const {
        if (!_hasWeather) return "";
        char buf[24];
        snprintf(buf, sizeof(buf), "%.1fC %s", _weatherTemp, _weatherCond.c_str());
        return String(buf);
    }

    // -----------------------------------------------------------------------
    // FACE 0: Standard Minimal Digital + Weather Icon & Condition
    // -----------------------------------------------------------------------
    void _drawStandardDigital(Adafruit_ST7789 &tft, const struct tm &tmNow) {
        char timeStr[12];
        snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);

        char dateStr[20];
        snprintf(dateStr, sizeof(dateStr), "%02d-%02d-%04d", tmNow.tm_mday, tmNow.tm_mon + 1, tmNow.tm_year + 1900);

        // Header status
        if (NetManager::isApMode()) {
            tft.setTextColor(COLOR_GOLD, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(10, 10);
            tft.print("AP: Companion-Setup");
            tft.setCursor(10, 24);
            tft.print("IP: 192.168.4.1");
        } else {
            tft.setTextColor(NetManager::isConnected() ? ST77XX_GREEN : ST77XX_RED, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(15, 15);
            tft.print(NetManager::isConnected() ? "Wi-Fi OK" : "Sin Wi-Fi");
        }

        // Time (Centered X)
        int timeX = (240 - ((int)strlen(timeStr) * 24)) / 2;
        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tft.setTextSize(4);
        tft.setCursor(timeX, 85);
        tft.print(timeStr);

        // Date (Centered X)
        int dateX = (240 - ((int)strlen(dateStr) * 12)) / 2;
        tft.setTextColor(0x8410, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setCursor(dateX, 140);
        tft.print(dateStr);

        // Weather Complication with Icon (Separated X margin)
        String wStr = _getWeatherSummaryStr();
        if (wStr.length() > 0) {
            int wX = (240 - (((int)wStr.length() * 12) + 24)) / 2;
            _drawWeatherIcon(tft, wX, 182);
            tft.setTextColor(COLOR_GREEN, ST77XX_BLACK);
            tft.setTextSize(2);
            tft.setCursor(wX + 24, 180);
            tft.print(wStr);
        }
    }

    // -----------------------------------------------------------------------
    // FACE 1: Apple / Nike Giant Bold Duo-Color + Weather Icon
    // -----------------------------------------------------------------------
    void _drawGiantBoldDuoColor(Adafruit_ST7789 &tft, const struct tm &tmNow) {
        char hrStr[4], minStr[4];
        snprintf(hrStr, sizeof(hrStr), "%02d", tmNow.tm_hour);
        snprintf(minStr, sizeof(minStr), "%02d", tmNow.tm_min);

        // Hours in Electric Blue
        int hrX = (240 - ((int)strlen(hrStr) * 60)) / 2;
        tft.setTextColor(COLOR_BLUE, ST77XX_BLACK);
        tft.setTextSize(10);
        tft.setCursor(hrX, 10);
        tft.print(hrStr);

        // Minutes in Vibrant Coral Red
        int minX = (240 - ((int)strlen(minStr) * 60)) / 2;
        tft.setTextColor(COLOR_CORAL, ST77XX_BLACK);
        tft.setTextSize(10);
        tft.setCursor(minX, 115);
        tft.print(minStr);

        // Weather Badge Footer with Icon (Separated X margin)
        String wStr = _getWeatherSummaryStr();
        if (wStr.length() > 0) {
            int wX = (240 - (((int)wStr.length() * 6) + 20)) / 2;
            _drawWeatherIcon(tft, wX, 222);
            tft.setTextColor(COLOR_CREAM, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(wX + 20, 222);
            tft.print(wStr);
        }
    }

    // -----------------------------------------------------------------------
    // FACE 2: Classic Analog Chronograph + Weather Complication Icon
    // -----------------------------------------------------------------------
    void _drawAnalogChronograph(Adafruit_ST7789 &tft, const struct tm &tmNow) {
        int cx = 120, cy = 120, r = 105;

        if (tmNow.tm_min != _lastDrawMin) {
            _lastDrawMin = tmNow.tm_min;
            tft.fillScreen(ST77XX_BLACK);

            // 12 Hour Ticks
            for (int i = 0; i < 12; i++) {
                float angle = i * 30.0f * (M_PI / 180.0f);
                int x1 = cx + (r - 12) * sin(angle);
                int y1 = cy - (r - 12) * cos(angle);
                int x2 = cx + r * sin(angle);
                int y2 = cy - r * cos(angle);
                tft.drawLine(x1, y1, x2, y2, ST77XX_WHITE);
            }

            // Sub-dials (Day & Date)
            tft.drawCircle(75, 120, 22, 0x5AD6);
            tft.drawCircle(165, 120, 22, 0x5AD6);

            const char *dayTxt = DAYS_ES[tmNow.tm_wday];
            int dayX = 75 - ((strlen(dayTxt) * 6) / 2);
            tft.setTextColor(COLOR_CREAM);
            tft.setTextSize(1);
            tft.setCursor(dayX, 117);
            tft.print(dayTxt);

            char dateTxt[4];
            snprintf(dateTxt, sizeof(dateTxt), "%d", tmNow.tm_mday);
            int mX = 165 - ((strlen(dateTxt) * 6) / 2);
            tft.setCursor(mX, 117);
            tft.print(dateTxt);

            // Weather Complication with Icon at Bottom Arc (Separated X margin)
            String wStr = _getWeatherSummaryStr();
            if (wStr.length() > 0) {
                int wX = 120 - ((((int)wStr.length() * 6) + 20) / 2);
                _drawWeatherIcon(tft, wX, 175);
                tft.setTextColor(COLOR_GOLD);
                tft.setTextSize(1);
                tft.setCursor(wX + 20, 175);
                tft.print(wStr);
            }

            _prevSx = _prevSy = -1;
            _prevMx = _prevMy = -1;
            _prevHx = _prevHy = -1;
        }

        // Calculate hand angles
        float hourAngle = (tmNow.tm_hour % 12 + tmNow.tm_min / 60.0f) * 30.0f * (M_PI / 180.0f);
        float minAngle  = (tmNow.tm_min + tmNow.tm_sec / 60.0f) * 6.0f * (M_PI / 180.0f);
        float secAngle  = (tmNow.tm_sec) * 6.0f * (M_PI / 180.0f);

        int hx = cx + 55 * sin(hourAngle);
        int hy = cy - 55 * cos(hourAngle);

        int mx = cx + 78 * sin(minAngle);
        int my = cy - 78 * cos(minAngle);

        int sx = cx + 85 * sin(secAngle);
        int sy = cy - 85 * cos(secAngle);

        // Erase previous second hand line only (ZERO flicker!)
        if (_prevSx != -1) {
            tft.drawLine(cx, cy, _prevSx, _prevSy, ST77XX_BLACK);
        }

        // Redraw hands
        tft.drawLine(cx, cy, hx, hy, ST77XX_WHITE);
        tft.drawLine(cx+1, cy+1, hx+1, hy+1, ST77XX_WHITE);
        tft.drawLine(cx, cy, mx, my, ST77XX_WHITE);
        tft.drawLine(cx, cy, sx, sy, ST77XX_RED);
        tft.fillCircle(cx, cy, 3, ST77XX_RED);

        _prevSx = sx; _prevSy = sy;
        _prevMx = mx; _prevMy = my;
        _prevHx = hx; _prevHy = hy;
    }

    // -----------------------------------------------------------------------
    // FACE 3: Retro Striped / Art Deco Stacked + Weather Icon Footer
    // -----------------------------------------------------------------------
    void _drawRetroStriped(Adafruit_ST7789 &tft, const struct tm &tmNow) {
        char hrStr[4], minStr[4];
        snprintf(hrStr, sizeof(hrStr), "%02d", tmNow.tm_hour);
        snprintf(minStr, sizeof(minStr), "%02d", tmNow.tm_min);

        // Top Row: Hours
        int hrX = (240 - ((int)strlen(hrStr) * 42)) / 2;
        tft.setTextColor(COLOR_CREAM, ST77XX_BLACK);
        tft.setTextSize(7);
        tft.setCursor(hrX, 15);
        tft.print(hrStr);

        // Bottom Row: Minutes
        int minX = (240 - ((int)strlen(minStr) * 42)) / 2;
        tft.setTextColor(COLOR_GOLD, ST77XX_BLACK);
        tft.setTextSize(7);
        tft.setCursor(minX, 105);
        tft.print(minStr);

        // Footer Date in Spanish
        char dateStr[24];
        snprintf(dateStr, sizeof(dateStr), "%s * %s %d", DAYS_ES[tmNow.tm_wday], MONTHS_ES[tmNow.tm_mon], tmNow.tm_mday);

        int dateX = (240 - ((int)strlen(dateStr) * 12)) / 2;
        tft.setTextColor(0x8410, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setCursor(dateX, 182);
        tft.print(dateStr);

        // Weather Footer with Icon (Separated X margin)
        String wStr = _getWeatherSummaryStr();
        if (wStr.length() > 0) {
            int wX = (240 - (((int)wStr.length() * 6) + 20)) / 2;
            _drawWeatherIcon(tft, wX, 215);
            tft.setTextColor(COLOR_GREEN, ST77XX_BLACK);
            tft.setTextSize(1);
            tft.setCursor(wX + 20, 215);
            tft.print(wStr);
        }
    }
};
