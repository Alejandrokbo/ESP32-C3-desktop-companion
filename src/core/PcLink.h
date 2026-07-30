#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

// Reads newline-delimited JSON from Serial (USB CDC), sent by the
// pc-agent/companion_agent.py script. Expected line shape:
//   {"cpu":23.4,"ram":51.2,"gpu":12.0,"gpu_temp":54.0}
// Any subset of fields is fine; missing fields keep their last known value.
// If nothing is received for STALE_MS, isOnline() goes false so pages can
// show a "PC offline" state instead of frozen numbers.
class PcLink {
public:
    void begin(unsigned long baud = 115200) {
        Serial.begin(baud);
    }

    void update() {
        while (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                _parseLine(_lineBuf);
                _lineBuf = "";
            } else if (c != '\r') {
                _lineBuf += c;
                if (_lineBuf.length() > 512) _lineBuf = ""; // guard against garbage
            }
        }
    }

    bool isOnline() const {
        return _lastRxMs != 0 && (millis() - _lastRxMs) < STALE_MS;
    }

    uint32_t claudeTokens() const { return _claudeTokens; }
    float claude5hPct() const { return _claude5hPct; }
    String claude5hReset() const { return _claude5hReset; }
    float claudeWeekPct() const { return _claudeWeekPct; }
    String claudeWeekReset() const { return _claudeWeekReset; }
    String claudeStatus() const { return _claudeStatus; }
    String clawMood() const { return _clawMood; }

private:
    static constexpr uint32_t STALE_MS = 5000;

    void _parseLine(const String &line) {
        if (line.isEmpty()) return;
        JsonDocument doc;
        if (deserializeJson(doc, line) != DeserializationError::Ok) return;

        if (doc["claude_tokens"].is<uint32_t>()) _claudeTokens = doc["claude_tokens"].as<uint32_t>();
        if (doc["claude_5h_pct"].is<float>()) _claude5hPct = doc["claude_5h_pct"].as<float>();
        if (doc["claude_5h_reset"].is<const char*>()) _claude5hReset = doc["claude_5h_reset"].as<String>();
        if (doc["claude_week_pct"].is<float>()) _claudeWeekPct = doc["claude_week_pct"].as<float>();
        if (doc["claude_week_reset"].is<const char*>()) _claudeWeekReset = doc["claude_week_reset"].as<String>();
        if (doc["claude_status"].is<const char*>()) _claudeStatus = doc["claude_status"].as<String>();
        if (doc["claw_mood"].is<const char*>()) _clawMood = doc["claw_mood"].as<String>();

        _lastRxMs = millis();
    }

    String _lineBuf;
    uint32_t _lastRxMs = 0;

    uint32_t _claudeTokens = 0;
    float _claude5hPct = 23.0f;
    String _claude5hReset = "2h 15m";
    float _claudeWeekPct = 17.0f;
    String _claudeWeekReset = "4d 22h";
    String _claudeStatus = "Idle";
    String _clawMood = "happy";
};
