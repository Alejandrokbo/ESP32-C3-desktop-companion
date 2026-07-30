#pragma once
#include "../core/Page.h"
#include "../core/PcLink.h"

// Claude Code Quotas & Rate Limits Page (Full-Width Stacked Progress Bars).
// Zero-flicker differential text rendering with Spanish localization and "Reset:" labels.
class ClaudeUsagePage : public Page {
public:
    explicit ClaudeUsagePage(PcLink &link) : _link(link) {}

    const char *name() const override { return "Claude Code"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        _lastDrawMs = 0; // force immediate redraw
        _prevTokens = 0xFFFFFFFF;
        _prev5hPct = -1.0f;
        _prevWeekPct = -1.0f;
        _prevStatus = "";
        _prev5hReset = "";
        _prevWeekReset = "";
        _drawStaticLayout(tft);
    }

    void update(Adafruit_ST7789 &tft) override {
        uint32_t now = millis();
        if (now - _lastDrawMs < REDRAW_INTERVAL_MS) return;
        _lastDrawMs = now;

        bool online = _link.isOnline();

        // 1. Status Bar
        String status = online ? _link.claudeStatus() : "Desconectado";
        if (status != _prevStatus) {
            _prevStatus = status;
            uint16_t dotColor = !online ? COLOR_MUTED :
                               ((status == "Coding..." || status == "Programando...") ? COLOR_TERRACOTTA : 
                               ((status == "Thinking..." || status == "Pensando...") ? COLOR_LAVENDER : COLOR_AMBER));
            
            tft.fillCircle(20, 41, 3, dotColor);
            tft.setTextColor(dotColor, COLOR_BG);
            tft.setTextSize(1);
            tft.setCursor(72, 38);
            tft.print(status);
            // Pad spaces if string shortened
            for (int i = status.length(); i < 16; i++) tft.print(" ");
        }

        // 2. Tokens Spent Value
        uint32_t tokens = _link.claudeTokens();
        if (tokens != _prevTokens) {
            _prevTokens = tokens;
            char tokStr[20];
            if (tokens >= 1000000) {
                snprintf(tokStr, sizeof(tokStr), "%.2f M", tokens / 1000000.0f);
            } else if (tokens >= 1000) {
                snprintf(tokStr, sizeof(tokStr), "%.1f K", tokens / 1000.0f);
            } else {
                snprintf(tokStr, sizeof(tokStr), "%lu", (unsigned long)tokens);
            }

            tft.setTextColor(COLOR_CREAM, COLOR_BG);
            tft.setTextSize(3);
            tft.setCursor(18, 64);
            tft.print(tokStr);
            for (int i = strlen(tokStr); i < 10; i++) tft.print(" ");
        }

        // 3. Card 1: 5-Hour / Session Quota Bar (Stacked Top)
        _updateQuotaCard(tft, 12, 98, 216, 64,
                          _link.claude5hPct(), 
                          _link.claude5hReset(), 
                          COLOR_TERRACOTTA,
                          _prev5hPct,
                          _prev5hReset);

        // 4. Card 2: Weekly Quota Bar (Stacked Bottom)
        _updateQuotaCard(tft, 12, 168, 216, 64,
                          _link.claudeWeekPct(), 
                          _link.claudeWeekReset(), 
                          COLOR_LAVENDER,
                          _prevWeekPct,
                          _prevWeekReset);
    }

private:
    static constexpr uint32_t REDRAW_INTERVAL_MS = 250;

    // Official Anthropic Palette (565 hex)
    static constexpr uint16_t COLOR_BG         = 0x1082; // Dark Obsidian (#101012)
    static constexpr uint16_t COLOR_HEADER     = 0x2104; // Dark Charcoal (#212124)
    static constexpr uint16_t COLOR_CARD       = 0x1925; // Dark Slate Card (#19191C)
    static constexpr uint16_t COLOR_TERRACOTTA = 0xE471; // Claude Terracotta / Salmon (#E58C8A)
    static constexpr uint16_t COLOR_LAVENDER   = 0xA59C; // Claude Lavender Blue (#A4B2E6)
    static constexpr uint16_t COLOR_AMBER      = 0xFBE0; // Warm Amber / Gold (#F59E0B)
    static constexpr uint16_t COLOR_CREAM      = 0xF77D; // Off-White Cream (#F4EFE6)
    static constexpr uint16_t COLOR_MUTED      = 0x8410; // Muted Grey (#848488)
    static constexpr uint16_t COLOR_TRACK      = 0x31A6; // Dark Track Gray

    PcLink &_link;
    uint32_t _lastDrawMs = 0;

    uint32_t _prevTokens = 0xFFFFFFFF;
    float _prev5hPct = -1.0f;
    float _prevWeekPct = -1.0f;
    String _prevStatus;
    String _prev5hReset;
    String _prevWeekReset;

    void _drawStaticLayout(Adafruit_ST7789 &tft) {
        tft.fillScreen(COLOR_BG);

        // Header Title Bar
        tft.fillRect(0, 0, 240, 26, COLOR_HEADER);
        tft.setTextColor(COLOR_CREAM);
        tft.setTextSize(2);
        tft.setCursor(45, 5);
        tft.print("CLAUDE CODE");
        tft.fillRect(0, 26, 240, 2, COLOR_TERRACOTTA);

        // Status Label
        tft.setTextColor(COLOR_MUTED, COLOR_BG);
        tft.setTextSize(1);
        tft.setCursor(28, 38);
        tft.print("ESTADO:");

        // Tokens Spent Label
        tft.setCursor(18, 52);
        tft.print("TOKENS USADOS");

        // Full-Width Stacked Card Wireframes
        _drawCardWireframe(tft, 12, 98, 216, 64, "SESION", COLOR_TERRACOTTA);
        _drawCardWireframe(tft, 12, 168, 216, 64, "SEMANAL", COLOR_LAVENDER);
    }

    void _drawCardWireframe(Adafruit_ST7789 &tft, int x, int y, int w, int h, const char *title, uint16_t accentColor) {
        tft.fillRoundRect(x, y, w, h, 6, COLOR_CARD);
        tft.drawRoundRect(x, y, w, h, 6, COLOR_HEADER);

        // Card Title (Left)
        tft.setTextColor(accentColor, COLOR_CARD);
        tft.setTextSize(1);
        tft.setCursor(x + 10, y + 8);
        tft.print(title);

        // Progress bar track
        tft.drawRoundRect(x + 10, y + 26, w - 20, 12, 3, COLOR_TRACK);

        // Reset: label (Changed from Rein: to Reset:)
        tft.setTextColor(COLOR_MUTED, COLOR_CARD);
        tft.setTextSize(1);
        tft.setCursor(x + 10, y + 46);
        tft.print("Reset:");
    }

    void _updateQuotaCard(Adafruit_ST7789 &tft, int x, int y, int w, int h,
                           float pct, const String &resetStr, uint16_t accentColor,
                           float &prevPct, String &prevResetStr) {
        pct = pct < 0.0f ? 0.0f : (pct > 100.0f ? 100.0f : pct);

        // Update Percentage Label (% USADO) at Top-Right
        if (pct != prevPct) {
            char pctBuf[16];
            snprintf(pctBuf, sizeof(pctBuf), "%.0f%% USADO", pct);
            
            tft.setTextColor(COLOR_CREAM, COLOR_CARD);
            tft.setTextSize(1);
            int txtX = (x + w - 10) - ((int)strlen(pctBuf) * 6);
            tft.setCursor(txtX, y + 8);
            tft.print(pctBuf);

            // Fill Progress Bar
            int barX = x + 10;
            int barY = y + 26;
            int barW = w - 20;
            int barH = 12;

            int fillW = (int)((barW - 4) * (pct / 100.0f));
            tft.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, COLOR_CARD);
            if (fillW > 0) {
                tft.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, accentColor);
            }
            prevPct = pct;
        }

        // Update Reset Countdown Label
        if (resetStr != prevResetStr) {
            prevResetStr = resetStr;
            tft.setTextColor(COLOR_CREAM, COLOR_CARD);
            tft.setTextSize(1);
            tft.setCursor(x + 50, y + 46);
            tft.print(resetStr);
            for (int i = resetStr.length(); i < 12; i++) tft.print(" ");
        }
    }
};
