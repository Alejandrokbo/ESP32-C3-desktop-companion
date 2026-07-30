#pragma once
#include "../core/Page.h"
#include "../core/NetManager.h"
#include "../config.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Weather page with automatic Open-Meteo fallback (100% FREE, Zero API Key required).
// Shows big weather icon + temperature + condition in Spanish + humidity.
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
                      "&current=temperature_2m,relative_humidity_2m,weather_code";

        Serial.println("[WeatherPage] Fetching weather from Open-Meteo...");
        http.setTimeout(10000); // 10s timeout to prevent -11 read timeouts
        http.setUserAgent("Mozilla/5.0 (ESP32-C3)");
        http.begin(url);
        
        int code = http.GET();
        Serial.printf("[WeatherPage] Open-Meteo HTTP code: %d\n", code);

        // Quick retry if transient network timeout (-11)
        if (code < 0) {
            http.end();
            delay(500);
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

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, http.getString());
        http.end();

        if (err) {
            Serial.println("[WeatherPage] JSON Parse error!");
            _haveData = false;
            _errorMsg = "Error JSON";
            return true;
        }

        _tempC = doc["current"]["temperature_2m"].as<float>();
        _humidity = doc["current"]["relative_humidity_2m"].as<int>();
        _wCode = doc["current"]["weather_code"].as<int>();

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
        Serial.printf("[WeatherPage] Success! Temp: %.1f C, Hum: %d%%, Cond: %s\n", _tempC, _humidity, _condition.c_str());
        return true;
    }

    void _drawBigWeatherIcon(Adafruit_ST7789 &tft, int x, int y) {
        if (!_haveData) return;

        if (_wCode == 0) {
            // Sun Icon (Yellow circle + 8 rays)
            tft.fillCircle(x + 16, y + 16, 10, ST77XX_YELLOW);
            tft.drawLine(x + 16, y + 2, x + 16, y + 5, ST77XX_YELLOW);
            tft.drawLine(x + 16, y + 27, x + 16, y + 30, ST77XX_YELLOW);
            tft.drawLine(x + 2, y + 16, x + 5, y + 16, ST77XX_YELLOW);
            tft.drawLine(x + 27, y + 16, x + 30, y + 16, ST77XX_YELLOW);
            
            tft.drawLine(x + 6, y + 6, x + 9, y + 9, ST77XX_YELLOW);
            tft.drawLine(x + 23, y + 23, x + 26, y + 26, ST77XX_YELLOW);
            tft.drawLine(x + 23, y + 6, x + 20, y + 9, ST77XX_YELLOW);
            tft.drawLine(x + 6, y + 23, x + 9, y + 20, ST77XX_YELLOW);
        } else if (_wCode >= 51 && _wCode <= 67) {
            // Rain Icon (Cloud + 3 Cyan Rain Drops)
            tft.fillCircle(x + 10, y + 12, 7, 0xD67A);
            tft.fillCircle(x + 20, y + 10, 9, 0xD67A);
            tft.fillRect(x + 5, y + 14, 20, 7, 0xD67A);
            tft.drawLine(x + 9, y + 23, x + 6, y + 28, 0x07FF);
            tft.drawLine(x + 16, y + 23, x + 13, y + 28, 0x07FF);
            tft.drawLine(x + 23, y + 23, x + 20, y + 28, 0x07FF);
        } else if (_wCode >= 95) {
            // Storm Icon (Cloud + Yellow Lightning)
            tft.fillCircle(x + 10, y + 10, 7, 0xD67A);
            tft.fillCircle(x + 20, y + 8, 9, 0xD67A);
            tft.fillRect(x + 5, y + 12, 20, 7, 0xD67A);
            tft.drawLine(x + 18, y + 20, x + 12, y + 25, ST77XX_YELLOW);
            tft.drawLine(x + 12, y + 25, x + 16, y + 29, ST77XX_YELLOW);
        } else {
            // Fluffy Cloud Icon (Nublado)
            tft.fillCircle(x + 10, y + 14, 8, 0xD67A);
            tft.fillCircle(x + 20, y + 11, 10, 0xD67A);
            tft.fillRect(x + 5, y + 15, 22, 8, 0xD67A);
        }
    }

    void _drawData(Adafruit_ST7789 &tft) {
        tft.fillRect(0, 0, 240, 240, ST77XX_BLACK);
        tft.setTextColor(ST77XX_WHITE);

        if (!NetManager::isConnected()) {
            tft.setTextSize(2);
            tft.setCursor(60, 110);
            tft.setTextColor(ST77XX_RED);
            tft.print("Sin Wi-Fi");
            return;
        }

        if (!_haveData) {
            tft.setTextSize(2);
            tft.setCursor(50, 110);
            if (!_errorMsg.isEmpty()) {
                tft.setTextColor(ST77XX_RED);
                tft.print(_errorMsg);
            } else {
                tft.setTextColor(0xFBE0);
                tft.print("Cargando...");
            }
            return;
        }

        // 1. Temperature (Centered X, Green)
        char tempStr[16];
        snprintf(tempStr, sizeof(tempStr), "%.1f%s",
                  _tempC, (strcmp(OWM_UNITS, "metric") == 0) ? "C" : "F");

        int tempX = (240 - ((int)strlen(tempStr) * 24)) / 2;
        tft.setTextColor(0x07E0); // Electric Green
        tft.setTextSize(4);
        tft.setCursor(tempX, 35);
        tft.print(tempStr);

        // 2. Large Weather Icon (32x32px centered at x = 104)
        _drawBigWeatherIcon(tft, 104, 85);

        // 3. Condition (Centered X, White)
        int condX = (240 - ((int)_condition.length() * 12)) / 2;
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(2);
        tft.setCursor(condX, 135);
        tft.print(_condition);

        // 4. Humidity (Centered X, Muted Grey)
        char humStr[24];
        snprintf(humStr, sizeof(humStr), "Humedad: %d%%", _humidity);
        int humX = (240 - ((int)strlen(humStr) * 12)) / 2;
        tft.setTextColor(0x8410); // Muted Grey
        tft.setTextSize(2);
        tft.setCursor(humX, 180);
        tft.print(humStr);
    }

    uint32_t _lastFetchMs = 0;
    bool _haveData = false;
    float _tempC = 0;
    int _humidity = 0;
    int _wCode = 0;
    String _condition;
    String _errorMsg;
};
