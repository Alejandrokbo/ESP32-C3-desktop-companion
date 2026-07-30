#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// ESP32-C3 Supermini pinout
//
// Avoided on purpose:
//   GPIO0, GPIO8, GPIO9  -> boot strapping pins (9 is also the BOOT button)
//   GPIO18, GPIO19       -> native USB D-/D+ (needed for flashing/Serial)
//   GPIO20, GPIO21       -> UART0 RX/TX (kept free for debugging over UART
//                           if you ever disable native USB CDC)
//
// SPI here is "soft-assigned": we remap the hardware SPI peripheral to these
// pins in DisplayManager, so any free GPIO works and we still get full
// hardware SPI speed (no bit-banging).
// ---------------------------------------------------------------------------

// --- ST7789 240x240 display (SPI) ---
constexpr uint8_t TFT_SCLK = 4;
constexpr uint8_t TFT_MOSI = 6;
constexpr uint8_t TFT_CS   = 7;
constexpr uint8_t TFT_DC   = 2;
constexpr uint8_t TFT_RST  = 10;
constexpr uint8_t TFT_BLK  = 3;   // backlight, PWM-capable

// --- TTP223 touch module (digital, active HIGH on touch) ---
constexpr uint8_t TOUCH_PIN = 5;

// GPIO1 is left free for a future second button / status LED.
