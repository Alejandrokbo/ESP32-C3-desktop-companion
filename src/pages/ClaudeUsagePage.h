#pragma once
#include "../core/Page.h"
#include "../core/PcLink.h"

// Claude Code Rate Limits & Quota page (Session & Weekly Quotas + Tokens Spent).
// Optimized with zero-flicker differential rendering and full Spanish localization.
class ClaudeUsagePage : public Page {
public:
    explicit ClaudeUsagePage(PcLink &link) : _link(link) {}

    const char *name() const override { return "Claude Code"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        _lastDrawMs = 0; // force immediate draw
        _drawStaticLayout(tft);
    }

    void update(Adafruit_ST7789 &tft) override {
        uint32_t now = millis();
        if (now - _lastDrawMs < REDRAW_INTERVAL_MS) return;
        _lastDrawMs = now;

        if (!_link.isOnline()) {
            if (_wasOnline) {
                _drawStaticLayout(tft);
                tft.setTextColor(COLOR_TERRACOTTA);
                tft.setTextSize(2);
                tft.setCursor(40, 110);
                tft.print("Sin conexion");
                _wasOnline = false;
            }
            return;
        }

        if (!_wasOnline) {
            _drawStaticLayout(tft);
            _wasOnline = true;
        }

        // 1. Status Bar (Update only text & dot)
        String status = _link.claudeStatus();
        uint16_t dotColor = (status == "Coding..." || status == "Programando...") ? COLOR_TERRACOTTA : 
                           ((status == "Thinking..." || status == "Pensando...") ? COLOR_LAVENDER : COLOR_AMBER);
        
        tft.fillCircle(22, 42, 3, dotColor);
        tft.fillRect(76, 39, 140, 12, COLOR_BG);
        tft.setTextColor(dotColor);
        tft.setTextSize(1);
        tft.setCursor(76, 39);
        tft.print(status);

        // 2. Tokens Spent Value
        uint32_t tokens = _link.claudeTokens();
        char tokStr[20];
        if (tokens >= 1000000) {
            snprintf(tokStr, sizeof(tokStr), "%.2f M", tokens / 1000000.0f);
        } else if (tokens >= 1000) {
            snprintf(tokStr, sizeof(tokStr), "%.1f K", tokens / 1000.0f);
        } else {
            snprintf(tokStr, sizeof(tokStr), "%lu", (unsigned long)tokens);
        }

        tft.fillRect(20, 70, 200, 24, COLOR_BG);
        tft.setTextColor(COLOR_CREAM);
        tft.setTextSize(3);
        tft.setCursor(20, 70);
        tft.print(tokStr);

        // 3. Card 1: Session Quota
        _updateQuotaWidget(tft, 14, 108, 102, 118, 
                           "", 
                           _link.claude5hPct(), 
                           _link.claude5hReset().c_str(), 
                           COLOR_TERRACOTTA);

        // 4. Card 2: Weekly Quota
        _updateQuotaWidget(tft, 124, 108, 102, 118, 
                           "SEMANAL", 
                           _link.claudeWeekPct(), 
                           _link.claudeWeekReset().c_str(), 
                           COLOR_LAVENDER);
    }

private:
    static constexpr uint32_t REDRAW_INTERVAL_MS = 500;

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
    bool _wasOnline = false;

    void _drawStaticLayout(Adafruit_ST7789 &tft) {
        tft.fillScreen(COLOR_BG);

        // Header Title Bar
        tft.fillRect(0, 0, 240, 28, COLOR_HEADER);
        tft.setTextColor(COLOR_CREAM);
        tft.setTextSize(2);
        tft.setCursor(45, 6);
        tft.print("CLAUDE CODE");
        tft.fillRect(0, 28, 240, 2, COLOR_TERRACOTTA);

        // Status Label
        tft.setTextColor(COLOR_MUTED);
        tft.setTextSize(1);
        tft.setCursor(24, 39);
        tft.print("ESTADO: ");

        // Tokens Spent Label
        tft.setCursor(20, 58);
        tft.print("TOKENS USADOS");

        // Card Wireframes
        _drawCardWireframe(tft, 14, 108, 102, 118, "", COLOR_TERRACOTTA);
        _drawCardWireframe(tft, 124, 108, 102, 118, "SEMANAL", COLOR_LAVENDER);
    }

    void _drawCardWireframe(Adafruit_ST7789 &tft, int x, int y, int w, int h, const char *title, uint16_t accentColor) {
        tft.fillRoundRect(x, y, w, h, 8, COLOR_CARD);
        tft.drawRoundRect(x, y, w, h, 8, COLOR_HEADER);

        if (title != nullptr && strlen(title) > 0) {
            tft.setTextColor(accentColor);
            tft.setTextSize(1);
            tft.setCursor(x + 10, y + 10);
            tft.print(title);
        }

        // Progress bar track
        tft.drawRoundRect(x + 10, y + 54, w - 20, 10, 3, COLOR_TRACK);

        // Rein: label
        tft.setTextColor(COLOR_MUTED);
        tft.setTextSize(1);
        tft.setCursor(x + 10, y + 76);
        tft.print("Rein:");
    }

    void _updateQuotaWidget(Adafruit_ST7789 &tft, int x, int y, int w, int h,
                            const char *title, float pct, const char *resetStr, uint16_t accentColor) {
        pct = pct < 0.0f ? 0.0f : (pct > 100.0f ? 100.0f : pct);

        // Clear & Draw Percentage Used Label (% USADO)
        tft.fillRect(x + 10, y + 26, w - 20, 16, COLOR_CARD);
        char pctStr[16];
        snprintf(pctStr, sizeof(pctStr), "%.0f%%", pct);
        tft.setTextColor(COLOR_CREAM);
        tft.setTextSize(2);
        tft.setCursor(x + 10, y + 26);
        tft.print(pctStr);

        tft.setTextColor(COLOR_MUTED);
        tft.setTextSize(1);
        tft.setCursor(x + 10 + (strlen(pctStr) * 12) + 2, y + 33);
        tft.print("USADO");

        // Fill Progress Bar
        int barX = x + 10;
        int barY = y + 54;
        int barW = w - 20;
        int barH = 10;

        tft.fillRect(barX + 2, barY + 2, barW - 4, barH - 4, COLOR_CARD);
        int fillW = (int)((barW - 4) * (pct / 100.0f));
        if (fillW > 0) {
            tft.fillRoundRect(barX + 2, barY + 2, fillW, barH - 4, 2, accentColor);
        }

        // Clear & Draw Reset Countdown Label
        tft.fillRect(x + 42, y + 76, w - 52, 10, COLOR_CARD);
        tft.setTextColor(COLOR_CREAM);
        tft.setCursor(x + 42, y + 76);
        tft.print(resetStr);
    }
};
