#pragma once
#include <vector>
#include "Page.h"

// Cycles through a fixed list of Page* in order. Does not own/delete the
// pages (main.cpp keeps them as static globals for their whole lifetime).
class PageManager {
public:
    void addPage(Page *page) { _pages.push_back(page); }

    void begin(Adafruit_ST7789 &tft) {
        if (_pages.empty()) return;
        _pages[_index]->onEnter(tft);
    }

    void update(Adafruit_ST7789 &tft) {
        if (_pages.empty()) return;
        _pages[_index]->update(tft);
    }

    void next(Adafruit_ST7789 &tft) {
        if (_pages.empty()) return;
        _pages[_index]->onExit();
        _index = (_index + 1) % _pages.size();
        _pages[_index]->onEnter(tft);
    }

    void longPress(Adafruit_ST7789 &tft) {
        if (_pages.empty()) return;
        _pages[_index]->onLongPress(tft);
    }

    const char *currentName() const {
        return _pages.empty() ? "none" : _pages[_index]->name();
    }

private:
    std::vector<Page *> _pages;
    size_t _index = 0;
};
