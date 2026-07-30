#pragma once
#include "../core/Page.h"
#include "../core/NetManager.h"
#include "../config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Weather page with automatic Open-Meteo fallback (100% FREE, Zero API Key required).
// Shows big weather icon (Sun/Moon) + temperature + condition in Spanish + humidity + centered Open-Meteo API credit.
class WeatherPage : public Page {
public:
    const char *name() const override { return "El Tiempo"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        tft.fillScreen(ST77XX_BLACK);
        _maybeFetch(true);
        _drawData(tft);
    }

    void onLongPress(Adafruit_ST7789 &tft) override {
        tft.fillScreen(ST77XX_BLACK);
        _maybeFetch(true);
        _drawData(tft);
    }

    void update(Adafruit_ST7789 &tft) override {
        bool refetched = _maybeFetch(false);
        if (refetched) _drawData(tft);
    }

private:
    static constexpr uint32_t POLL_INTERVAL_MS = 600000; // 10 minutes

    bool _maybeFetch(bool force) {
        if (!NetManager::isConnected()) return false;
        uint32_t now = millis();
        if (!force && _lastFetchMs != 0 && (now - _lastFetchMs) < POLL_INTERVAL_MS) {
            return false;
        }
        _lastFetchMs = now;
        return _fetchOpenMeteo();
    }

    // Open-Meteo: 100% Free Public Weather API with NO API KEY required!
    bool _fetchOpenMeteo() {
        HTTPClient http;
        String url = String("http://api.open-meteo.com/v1/forecast?latitude=") +
                      OWM_LAT + "&longitude=" + OWM_LON +
                      "&current=temperature_2m,relative_humidity_2m,weather_code,is_day";

        Serial.println("[WeatherPage] Fetching weather from Open-Meteo...");
        http.useHTTP10(true); // Disable Chunked Transfer Encoding for clean HTTP 1.0 JSON response
        http.setTimeout(10000);
        http.setUserAgent("Mozilla/5.0 (ESP32-C3)");
        http.begin(url);
        
        int code = http.GET();
        Serial.printf("[WeatherPage] Open-Meteo HTTP code: %d\n", code);

        if (code < 0) {
            http.end();
            delay(500);
            http.useHTTP10(true);
            http.begin(url);
            http.setTimeout(10000);
            http.setUserAgent("Mozilla/5.0 (ESP32-C3)");
            code = http.GET();
            Serial.printf("[WeatherPage] Retry HTTP code: %d\n", code);
        }

        if (code != 200) {
            http.end();
            _haveData = false;
            _errorMsg = "HTTP Err " + String(code);
            return true;
        }

        String payload = http.getString();
        http.end();

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, payload);

        if (err) {
            Serial.println("[WeatherPage] JSON Parse error!");
            _haveData = false;
            _errorMsg = "Error JSON";
            return true;
        }

        _tempC = doc["current"]["temperature_2m"].as<float>();
        _humidity = doc["current"]["relative_humidity_2m"].as<int>();
        _wCode = doc["current"]["weather_code"].as<int>();
        _isDay = doc["current"]["is_day"].as<int>() == 1;

        // Translate WMO Weather Code to Spanish
        if (_wCode == 0) _condition = "Despejado";
        else if (_wCode >= 1 && _wCode <= 3) _condition = "Nublado";
        else if (_wCode == 45 || _wCode == 48) _condition = "Niebla";
        else if (_wCode >= 51 && _wCode <= 67) _condition = "Lluvia";
        else if (_wCode >= 71 && _wCode <= 77) _condition = "Nieve";
        else if (_wCode >= 80 && _wCode <= 82) _condition = "Chubascos";
        else if (_wCode >= 95) _condition = "Tormenta";
        else _condition = "Templado";

        _haveData = true;
        _errorMsg = "";
        Serial.printf("[WeatherPage] Success! Temp: %.1f C, Hum: %d%%, Cond: %s, Day: %d\n", _tempC, _humidity, _condition.c_str(), _isDay);
        return true;
    }

    void _drawBigWeatherIcon(Adafruit_ST7789 &tft, int x, int y) {
        if (!_haveData) return;

        if (_wCode == 0) {
            if (_isDay) {
                // Sun Icon ☀️ (Yellow circle + 8 rays)
                tft.fillCircle(x + 16, y + 16, 10, ST77XX_YELLOW);
                tft.drawLine(x + 16, y + 2, x + 16, y + 5, ST77XX_YELLOW);
                tft.drawLine(x + 16, y + 27, x + 16, y + 30, ST77XX_YELLOW);
                tft.drawLine(x + 2, y + 16, x + 5, y + 16, ST77XX_YELLOW);
                tft.drawLine(x + 27, y + 16, x + 30, y + 16, ST77XX_YELLOW);
                
                tft.drawLine(x + 6, y + 6, x + 9, y + 9, ST77XX_YELLOW);
                tft.drawLine(x + 23, y + 23, x + 26, y + 26, ST77XX_YELLOW);
                tft.drawLine(x + 23, y + 6, x + 20, y + 9, ST77XX_YELLOW);
                tft.drawLine(x + 6, y + 23, x + 9, y + 20, ST77XX_YELLOW);
            } else {
                // Crescent Moon Icon 🌙 (Gold circle + cutout + stars)
                tft.fillCircle(x + 16, y + 16, 12, 0xFBE0);
                tft.fillCircle(x + 21, y + 12, 11, ST77XX_BLACK);
                tft.drawPixel(x + 6, y + 8, ST77XX_WHITE);
                tft.drawPixel(x + 26, y + 24, ST77XX_WHITE);
            }
        } else if (_wCode >= 51 && _wCode <= 67) {
            // Rain Icon (Cloud + 3 Cyan Rain Drops)
            tft.fillCircle(x + 10, y + 12, 7, 0xD67A);
            tft.fillCircle(x + 20, y + 10, 9, 0xD67A);
            tft.fillRect(x + 5, y + 14, 20, 7, 0xD67A);
            tft.drawLine(x + 9, y + 23, x + 6, y + 28, 0x07FF);
            tft.drawLine(x + 16, y + 23, x + 13, y + 28, 0x07FF);
            tft.drawLine(x + 23, y + 23, x + 20, y + 28, 0x07FF);
        } else if (_wCode >= 95) {
            // Storm Icon (Cloud + Lightning)
            tft.fillCircle(x + 10, y + 10, 7, 0xD67A);
            tft.fillCircle(x + 20, y + 8, 9, 0xD67A);
            tft.fillRect(x + 5, y + 12, 20, 7, 0xD67A);
            tft.drawLine(x + 18, y + 20, x + 12, y + 26, ST77XX_YELLOW);
            tft.drawLine(x + 12, y + 26, x + 16, y + 29, ST77XX_YELLOW);
        } else {
            // Cloud Icon (Cloud)
            tft.fillCircle(x + 10, y + 12, 7, 0xD67A);
            tft.fillCircle(x + 20, y + 10, 9, 0xD67A);
            tft.fillRect(x + 5, y + 14, 20, 8, 0xD67A);
        }
    }

    void _drawData(Adafruit_ST7789 &tft) {
        tft.fillScreen(ST77XX_BLACK);

        if (!_haveData) {
            tft.setTextColor(ST77XX_RED);
            tft.setTextSize(2);
            tft.setCursor(30, 110);
            tft.print(_errorMsg.isEmpty() ? "Cargando..." : _errorMsg);
            return;
        }

        // 32x32px Big Weather Icon (Centered at X=104, Y=25)
        _drawBigWeatherIcon(tft, 104, 25);

        // Main Temperature in Electric Green (Centered)
        char tempBuf[16];
        snprintf(tempBuf, sizeof(tempBuf), "%.1f C", _tempC);
        int tX = (240 - ((int)strlen(tempBuf) * 24)) / 2;
        tft.setTextColor(0x07E0);
        tft.setTextSize(4);
        tft.setCursor(tX, 75);
        tft.print(tempBuf);

        // Condition string in Spanish (Centered)
        int cX = (240 - ((int)_condition.length() * 12)) / 2;
        tft.setTextColor(ST77XX_CYAN);
        tft.setTextSize(2);
        tft.setCursor(cX, 125);
        tft.print(_condition);

        // Humidity in Bright White Size 2 (Centered)
        char humBuf[20];
        snprintf(humBuf, sizeof(humBuf), "Humedad: %d%%", _humidity);
        int hX = (240 - ((int)strlen(humBuf) * 12)) / 2;
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.setCursor(hX, 165);
        tft.print(humBuf);

        // Footer note: Centered Open-Meteo API credit
        const char *credit = "Open-Meteo API";
        int oX = (240 - ((int)strlen(credit) * 6)) / 2;
        tft.setTextColor(0x5AD6);
        tft.setTextSize(1);
        tft.setCursor(oX, 210);
        tft.print(credit);
    }

    uint32_t _lastFetchMs = 0;
    bool _haveData = false;
    float _tempC = 0.0f;
    int _humidity = 0;
    int _wCode = 0;
    bool _isDay = true;
    String _condition = "Cargando...";
    String _errorMsg = "";
};
