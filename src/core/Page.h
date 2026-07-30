#pragma once
#include <Adafruit_ST7789.h>

// Base interface every screen/functionality implements. PageManager owns the
// lifecycle: onEnter() when it becomes visible, update() every loop tick
// while visible, onExit() right before switching away.
class Page {
public:
    virtual ~Page() = default;

    virtual const char *name() const = 0;

    // Called once when the page becomes active. Good place to force a full
    // redraw so stale content from the previous page never lingers.
    virtual void onEnter(Adafruit_ST7789 &tft) { (void)tft; }

    // Called once right before switching to another page.
    virtual void onExit() {}

    // Called every loop iteration while this page is active. Implementations
    // should throttle their own redraws/network calls internally (millis()
    // based), since this can be called very frequently.
    virtual void update(Adafruit_ST7789 &tft) = 0;

    // Long touch-press while this page is active: "refresh now" semantics
    // (force NTP resync, force weather re-fetch, etc). Optional to implement.
    virtual void onLongPress(Adafruit_ST7789 &tft) { (void)tft; }
};
