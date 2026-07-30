#pragma once
#include "../core/Page.h"
#include <Arduino.h>

// Matrix Digital Rain, Star Wars Crawl, Pac-Man & AFK Sleeping Mascot Page.
// Long-press: Cycle modes (Matrix Rain -> Star Wars Crawl -> Pac-Man Arcade -> AFK Sleep).
class MatrixRefresherPage : public Page {
public:
    const char *name() const override { return "Matrix & Arcade"; }

    void onEnter(Adafruit_ST7789 &tft) override {
        tft.fillScreen(ST77XX_BLACK);
        _mode = MODE_MATRIX;
        _initMatrix();
        _lastUpdateMs = 0;
    }

    void onLongPress(Adafruit_ST7789 &tft) override {
        _mode = (_mode + 1) % 4; // Cycle 0: Matrix, 1: Star Wars, 2: Pacman, 3: AFK Sleep
        tft.fillScreen(ST77XX_BLACK);
        if (_mode == MODE_MATRIX) _initMatrix();
        else if (_mode == MODE_STARWARS) _initStarWars(tft);
        else if (_mode == MODE_PACMAN) _initPacman(tft);
        else if (_mode == MODE_AFK) _initAfk(tft);
        _lastUpdateMs = 0;
    }

    void update(Adafruit_ST7789 &tft) override {
        uint32_t now = millis();
        
        if (_mode == MODE_MATRIX) {
            if (now - _lastUpdateMs < 50) return;
            _lastUpdateMs = now;
            _updateMatrix(tft);
        } else if (_mode == MODE_STARWARS) {
            if (now - _lastUpdateMs < 70) return;
            _lastUpdateMs = now;
            _updateStarWars(tft);
        } else if (_mode == MODE_PACMAN) {
            if (now - _lastUpdateMs < 80) return;
            _lastUpdateMs = now;
            _updatePacman(tft);
        } else if (_mode == MODE_AFK) {
            if (now - _lastUpdateMs < 120) return;
            _lastUpdateMs = now;
            _updateAfk(tft);
        }
    }

private:
    static constexpr int MODE_MATRIX   = 0;
    static constexpr int MODE_STARWARS = 1;
    static constexpr int MODE_PACMAN   = 2;
    static constexpr int MODE_AFK      = 3;

    int _mode = MODE_MATRIX;
    uint32_t _lastUpdateMs = 0;

    // Palette
    static constexpr uint16_t COLOR_MATRIX_HEAD = 0xAFE5; // Bright White-Green
    static constexpr uint16_t COLOR_MATRIX_GREEN= 0x07E0; // Neon Green
    static constexpr uint16_t COLOR_PACMAN       = 0xFFE0; // Yellow
    static constexpr uint16_t COLOR_SW_BLUE      = 0x4DFE; // Star Wars Intro Blue (#4BD5FF)
    static constexpr uint16_t COLOR_SW_YELLOW    = 0xFFE0; // Star Wars Crawl Yellow
    static constexpr uint16_t COLOR_TERRACOTTA = 0xE471; // Claude Terracotta / Salmon (#E58C8A)
    static constexpr uint16_t COLOR_LAVENDER   = 0xA59C; // Claude Lavender Blue (#A4B2E6)
    static constexpr uint16_t COLOR_WOOD       = 0xA280; // Warm Wood Brown
    static constexpr uint16_t COLOR_CREAM      = 0xF77D; // Off-White Cream

    // -----------------------------------------------------------------------
    // MODE 0: MATRIX DIGITAL RAIN (Screen Refresher)
    // -----------------------------------------------------------------------
    static constexpr int COLS = 16;
    int _dropY[COLS];
    int _dropLength[COLS];
    int _dropSpeed[COLS];

    void _initMatrix() {
        for (int i = 0; i < COLS; i++) {
            _dropY[i] = random(-20, 0);
            _dropLength[i] = random(5, 12);
            _dropSpeed[i] = random(1, 3);
        }
    }

    void _updateMatrix(Adafruit_ST7789 &tft) {
        for (int i = 0; i < COLS; i++) {
            int x = i * 15;
            int headY = _dropY[i] * 12;

            int tailY = (_dropY[i] - _dropLength[i]) * 12;
            if (tailY >= 0 && tailY < 240) {
                tft.fillRect(x, tailY, 12, 12, ST77XX_BLACK);
            }

            if (headY >= 0 && headY < 240) {
                char ch = (char)random(33, 126);
                tft.setTextColor(COLOR_MATRIX_HEAD, ST77XX_BLACK);
                tft.setTextSize(1);
                tft.setCursor(x + 2, headY + 2);
                tft.print(ch);
            }

            int bodyY = (_dropY[i] - 1) * 12;
            if (bodyY >= 0 && bodyY < 240) {
                char ch = (char)random(33, 126);
                tft.setTextColor(COLOR_MATRIX_GREEN, ST77XX_BLACK);
                tft.setTextSize(1);
                tft.setCursor(x + 2, bodyY + 2);
                tft.print(ch);
            }

            _dropY[i] += _dropSpeed[i];
            if ((_dropY[i] - _dropLength[i]) * 12 > 240) {
                _dropY[i] = 0;
                _dropLength[i] = random(5, 12);
                _dropSpeed[i] = random(1, 3);
            }
        }
    }

    // -----------------------------------------------------------------------
    // MODE 1: STAR WARS OPENING CRAWL (Large Size 2 Text + Twinkling Starfield)
    // -----------------------------------------------------------------------
    int _crawlY = 240;
    static constexpr int SW_LINE_COUNT = 30;
    const char *SW_CREDITS[SW_LINE_COUNT] = {
        "A long time ago",
        "in a galaxy far,",
        "far away...",
        "",
        "DESKTOP",
        "COMPANION",
        "------------------",
        "Alejandro",
        "se aburrio y",
        "decidio crear este",
        "Companion Monitor.",
        "",
        "--- HARDWARE ---",
        "ESP32-C3 Supermini",
        "ST7789 240x240 LCD",
        "TTP223 Touch",
        "Case impreso en 3D",
        "Bambulab P2S",
        "",
        "--- FUNCIONES ---",
        "4 Esferas de Reloj",
        "Clima OpenMeteo",
        "Metricas Claude",
        "Mascota Robot",
        "Lluvia Matrix 60fps",
        "Pac-Man Arcade",
        "Modo AFK Sleep",
        "",
        "Made by",
        "Alejo Kbo"
    };

    void _initStarWars(Adafruit_ST7789 &tft) {
        tft.fillScreen(ST77XX_BLACK);
        _crawlY = 240;
    }

    void _updateStarWars(Adafruit_ST7789 &tft) {
        // Redraw twinkling starfield background (Cyan & White stars)
        for (int i = 0; i < 35; i++) {
            int sx = (i * 37) % 240;
            int sy = (i * 73) % 240;
            uint16_t starColor = (i % 3 == 0) ? COLOR_SW_BLUE : (i % 2 == 0 ? ST77XX_WHITE : 0x39FF);
            tft.drawPixel(sx, sy, starColor);
        }

        int currY = _crawlY;

        for (int i = 0; i < SW_LINE_COUNT; i++) {
            int lineH = 18;

            if (currY >= -16 && currY <= 240) {
                const char *line = SW_CREDITS[i];
                int len = strlen(line);

                // Erase trailing 2px strip below text line
                if (currY + lineH >= 0 && currY + lineH <= 240) {
                    tft.fillRect(0, currY + lineH, 240, 2, ST77XX_BLACK);
                }

                if (len > 0) {
                    // Intro and final Made by signature in Cyan Blue
                    uint16_t color = (i <= 2 || i >= (SW_LINE_COUNT - 2)) ? COLOR_SW_BLUE : COLOR_SW_YELLOW;
                    tft.setTextColor(color, ST77XX_BLACK);
                    tft.setTextSize(2);
                    int x = (240 - (len * 12)) / 2;
                    tft.setCursor(x, currY);
                    tft.print(line);
                }
            }
            currY += lineH + 6;
        }

        _crawlY -= 2;
        if (_crawlY < -(SW_LINE_COUNT * 24 + 20)) {
            tft.fillScreen(ST77XX_BLACK);
            _crawlY = 240; // Loop crawl
        }
    }

    // -----------------------------------------------------------------------
    // MODE 2: PAC-MAN ARCADE (Ghost Color Cycling + Zero Trail Clean Erase)
    // -----------------------------------------------------------------------
    int _pacX = 20;
    int _pacY = 120;
    int _ghostX = 180;
    int _ghostY = 120;
    int _mouthAngle = 0;
    int _ghostColorIdx = 0;

    void _initPacman(Adafruit_ST7789 &tft) {
        tft.fillScreen(ST77XX_BLACK);
        _pacX = 20;
        _pacY = 120;
        _ghostX = 180;
        _ghostY = 120;
        _ghostColorIdx = 0;

        tft.setTextColor(COLOR_PACMAN);
        tft.setTextSize(2);
        tft.setCursor(45, 10);
        tft.print("PAC-MAN ARCADE");

        tft.drawRoundRect(10, 35, 220, 170, 8, 0x001F);
        tft.drawRoundRect(12, 37, 216, 166, 6, 0x001F);

        for (int x = 30; x < 210; x += 20) {
            tft.fillCircle(x, 120, 3, 0xFBE0);
        }
    }

    void _updatePacman(Adafruit_ST7789 &tft) {
        tft.fillCircle(_pacX, _pacY, 11, ST77XX_BLACK);
        tft.fillRect(_ghostX - 12, _ghostY - 12, 24, 24, ST77XX_BLACK);

        _pacX += 4;
        _ghostX += 4;

        if (_pacX > 210) {
            _pacX = 20;
            _ghostX = -30;
            _ghostColorIdx = (_ghostColorIdx + 1) % 4;

            for (int x = 30; x < 210; x += 20) {
                tft.fillCircle(x, 120, 3, 0xFBE0);
            }
        }

        tft.fillCircle(_pacX, _pacY, 9, COLOR_PACMAN);
        _mouthAngle = (_mouthAngle + 1) % 2;
        if (_mouthAngle == 1) {
            tft.fillTriangle(_pacX, _pacY, _pacX + 10, _pacY - 5, _pacX + 10, _pacY + 5, ST77XX_BLACK);
        }

        const uint16_t ghostColors[4] = { 0xF800, 0xF81F, 0x07FF, 0xFC00 };

        if (_ghostX > 10 && _ghostX < 230) {
            uint16_t ghostColor = ghostColors[_ghostColorIdx];
            tft.fillCircle(_ghostX, _ghostY - 2, 8, ghostColor);
            tft.fillRect(_ghostX - 8, _ghostY - 2, 16, 8, ghostColor);
            tft.fillCircle(_ghostX - 3, _ghostY - 3, 2, ST77XX_WHITE);
            tft.fillCircle(_ghostX + 3, _ghostY - 3, 2, ST77XX_WHITE);
            tft.drawPixel(_ghostX - 2, _ghostY - 3, ST77XX_BLUE);
            tft.drawPixel(_ghostX + 4, _ghostY - 3, ST77XX_BLUE);
        }
    }

    // -----------------------------------------------------------------------
    // MODE 3: AFK MODE (Claude Mascot Sleeping Cozy in Bed with Floating Zzz)
    // -----------------------------------------------------------------------
    int _zzzY = 120;

    void _initAfk(Adafruit_ST7789 &tft) {
        tft.fillScreen(ST77XX_BLACK);
        _zzzY = 120;

        // Header Title
        tft.setTextColor(COLOR_TERRACOTTA, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setCursor(90, 15);
        tft.print("AFTPS");

        tft.setTextColor(0x8410, ST77XX_BLACK);
        tft.setTextSize(1);
        tft.setCursor(81, 38);
        tft.print("Away from TPS");

        // Night stars background
        for (int i = 0; i < 20; i++) {
            int sx = random(10, 230);
            int sy = random(50, 120);
            tft.drawPixel(sx, sy, COLOR_LAVENDER);
        }

        // Bed Frame (Wooden Headboard & Base)
        tft.fillRoundRect(20, 140, 24, 60, 4, COLOR_WOOD);   // Headboard Left
        tft.fillRoundRect(25, 155, 190, 45, 6, COLOR_WOOD);  // Bed Main Frame
        tft.fillRoundRect(205, 165, 15, 35, 4, COLOR_WOOD);  // Footboard Right

        // Pillow (Cream)
        tft.fillRoundRect(42, 148, 45, 24, 5, COLOR_CREAM);

        // Blanket / Duvet (Lavender Blue)
        tft.fillRoundRect(80, 155, 130, 40, 6, COLOR_LAVENDER);
        tft.drawRoundRect(80, 155, 130, 40, 6, ST77XX_WHITE);

        // Sleeping Claw Mascot Head resting on pillow
        _drawSleepingClaw(tft, 50, 142);
    }

    void _drawSleepingClaw(Adafruit_ST7789 &tft, int cx, int cy) {
        // Robot ears
        tft.fillRect(cx + 2, cy - 6, 4, 6, COLOR_TERRACOTTA);
        tft.fillRect(cx + 22, cy - 6, 4, 6, COLOR_TERRACOTTA);

        // Main Head (Salmon/Terracotta)
        tft.fillRoundRect(cx, cy, 28, 22, 5, COLOR_TERRACOTTA);

        // Screen Face (Dark Obsidian)
        tft.fillRoundRect(cx + 3, cy + 3, 22, 16, 3, 0x1082);

        // Closed Sleeping Eyes (- -)
        tft.drawLine(cx + 6, cy + 10, cx + 11, cy + 10, COLOR_TERRACOTTA);
        tft.drawLine(cx + 17, cy + 10, cx + 22, cy + 10, COLOR_TERRACOTTA);
    }

    void _updateAfk(Adafruit_ST7789 &tft) {
        // Erase old Zzz
        tft.fillRect(140, _zzzY - 2, 60, 20, ST77XX_BLACK);

        _zzzY -= 3;
        if (_zzzY < 60) _zzzY = 120;

        // Draw Floating Zzz's 💤
        tft.setTextColor(COLOR_CREAM, ST77XX_BLACK);
        tft.setTextSize(2);
        tft.setCursor(140, _zzzY);
        tft.print("Z");

        tft.setTextSize(1);
        tft.setCursor(160, _zzzY + 4);
        tft.print("z");

        tft.setCursor(175, _zzzY + 8);
        tft.print("z");
    }
};
