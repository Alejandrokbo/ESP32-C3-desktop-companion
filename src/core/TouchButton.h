#pragma once
#include <Arduino.h>
#include "Pins.h"

// Debounced reader for the TTP223 module. Reports two distinct events:
//  - tap:        press+release shorter than LONG_PRESS_MS  -> advance page
//  - long press: held longer than LONG_PRESS_MS             -> refresh page
// Call update() every loop iteration; then check wasTapped()/wasLongPressed().
class TouchButton {
public:
    void begin() {
        pinMode(TOUCH_PIN, INPUT); // TTP223 has its own output driver, no pull needed
    }

    void update() {
        _tapped = false;
        _longPressed = false;

        bool raw = digitalRead(TOUCH_PIN) == HIGH;
        uint32_t now = millis();

        if (raw != _lastRaw && (now - _lastEdgeMs) > DEBOUNCE_MS) {
            _lastEdgeMs = now;
            _lastRaw = raw;

            if (raw) {
                // Rising edge: touch started.
                _pressStartMs = now;
                _longFired = false;
            } else {
                // Falling edge: touch released.
                if (!_longFired) {
                    _tapped = true;
                }
            }
        }

        // Fire long-press once, while still held, without waiting for release.
        if (raw && !_longFired && (now - _pressStartMs) > LONG_PRESS_MS) {
            _longFired = true;
            _longPressed = true;
        }
    }

    bool wasTapped() const { return _tapped; }
    bool wasLongPressed() const { return _longPressed; }

private:
    static constexpr uint32_t DEBOUNCE_MS = 40;
    static constexpr uint32_t LONG_PRESS_MS = 700;

    bool _lastRaw = false;
    bool _longFired = false;
    bool _tapped = false;
    bool _longPressed = false;
    uint32_t _lastEdgeMs = 0;
    uint32_t _pressStartMs = 0;
};
