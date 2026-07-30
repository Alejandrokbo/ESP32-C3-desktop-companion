// Copy this file to "config.h" (same folder) and fill in your own values.
// config.h is gitignored on purpose so your credentials never get committed.
#pragma once

// ---------------------------------------------------------------------------
// Wi-Fi
// ---------------------------------------------------------------------------
#define WIFI_SSID     "your-wifi-name"
#define WIFI_PASSWORD "your-wifi-password"

// ---------------------------------------------------------------------------
// Clock page (NTP)
// ---------------------------------------------------------------------------
#define NTP_SERVER      "pool.ntp.org"
// UTC offset in seconds and DST offset in seconds, e.g. Madrid (CET/CEST):
#define GMT_OFFSET_SEC   3600
#define DAYLIGHT_OFFSET_SEC 3600

// ---------------------------------------------------------------------------
// Weather page (OpenWeatherMap, free tier "current weather" endpoint)
// Get a free API key at https://openweathermap.org/api
// ---------------------------------------------------------------------------
#define OWM_API_KEY   "your-openweathermap-api-key"
#define OWM_LAT       "40.4168"   // latitude, as string
#define OWM_LON       "-3.7038"  // longitude, as string
#define OWM_UNITS     "metric"    // "metric" (C) or "imperial" (F)
#define OWM_LANG      "en"
// How often to poll the API (milliseconds). Keep this generous, free tier
// allows 60 calls/minute but there is no need to hammer it for a desk toy.
#define OWM_POLL_INTERVAL_MS (10UL * 60UL * 1000UL)
