#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "../config.h"

// Wi-Fi Connection Manager with built-in Web Server Captive Portal.
// If Wi-Fi is not configured or fails to connect, creates an AP hotspot
// named "Companion-Setup" at http://192.168.4.1 for interactive setup.
namespace NetManager {

inline uint32_t _lastAttemptMs = 0;
inline bool _apMode = false;
inline WebServer _server(80);
inline DNSServer _dnsServer;
inline Preferences _prefs;

inline void _handleRoot() {
    Serial.println("[NetManager] Web Server HTTP request received -> serving setup page");
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                  "<title>Companion Setup</title><style>"
                  "body{font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;background:#101012;color:#F4EFE6;padding:20px;margin:0;display:flex;justify-content:center;}"
                  ".card{background:#19191C;border:1px solid #212124;padding:24px;border-radius:12px;max-width:360px;width:100%;box-shadow:0 8px 24px rgba(0,0,0,0.5);}"
                  "h2{color:#E58C8A;margin-top:0;font-size:22px;text-align:center;}"
                  "p{color:#848488;font-size:14px;line-height:1.4;text-align:center;}"
                  "label{display:block;margin-top:14px;color:#A4B2E6;font-size:13px;font-weight:600;}"
                  "input,select{width:100%;padding:10px;margin-top:6px;border-radius:6px;border:1px solid #313136;background:#101012;color:#FFF;box-sizing:border-box;font-size:14px;}"
                  "button{width:100%;padding:12px;margin-top:20px;background:#E58C8A;color:#101012;border:none;border-radius:6px;font-weight:bold;font-size:15px;cursor:pointer;}"
                  "button:hover{background:#F4EFE6;}"
                  "</style></head><body><div class='card'>"
                  "<h2>🤖 Companion Setup</h2>"
                  "<p>Selecciona tu red Wi-Fi e introduce la contraseña para conectar el dispositivo.</p>"
                  "<form action='/save' method='POST'>"
                  "<label>Red Wi-Fi (SSID):</label>"
                  "<select name='ssid'>";

    int n = WiFi.scanNetworks();
    if (n == 0) {
        html += "<option value=''>No se encontraron redes</option>";
    } else {
        for (int i = 0; i < n; ++i) {
            html += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
        }
    }

    html += "</select>"
            "<label>Contraseña:</label>"
            "<input type='password' name='pass' placeholder='Tu contraseña de Wi-Fi' required>"
            "<button type='submit'>Guardar y Conectar</button>"
            "</form></div></body></html>";

    _server.send(200, "text/html", html);
}

inline void _handleSave() {
    String ssid = _server.arg("ssid");
    String pass = _server.arg("pass");
    Serial.println("[NetManager] Saving new Wi-Fi credentials for: " + ssid);

    if (ssid.length() > 0) {
        _prefs.begin("wifi-config", false); // read-write mode to create namespace
        _prefs.putString("ssid", ssid);
        _prefs.putString("pass", pass);
        _prefs.end();

        String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
                      "<style>body{background:#101012;color:#E58C8A;font-family:sans-serif;text-align:center;padding:40px;}"
                      "h2{color:#F4EFE6;}</style></head><body>"
                      "<h2>✅ Configuración Guardada</h2>"
                      "<p>Reiniciando el Companion Monitor para conectar a <b>" + ssid + "</b>...</p>"
                      "</body></html>";
        _server.send(200, "text/html", html);
        delay(2000);
        ESP.restart();
    } else {
        _server.send(400, "text/plain", "SSID invalido");
    }
}

inline void _startAP() {
    _apMode = true;
    Serial.println("[NetManager] Connection failed or no credentials.");
    Serial.println("[NetManager] Starting AP 'Companion-Setup' on Channel 1 with Max TX Power (19.5dBm)...");
    
    WiFi.disconnect(true);
    delay(100);
    WiFi.mode(WIFI_AP_STA);
    WiFi.setTxPower(WIFI_POWER_19_5dBm); // Set maximum Wi-Fi transmit power
    WiFi.softAP("Companion-Setup", nullptr, 1, 0, 4); // Channel 1, SSID visible, max 4 clients

    IPAddress apIP = WiFi.softAPIP();
    Serial.print("[NetManager] AP Started. IP: ");
    Serial.println(apIP);

    _dnsServer.start(53, "*", apIP);

    _server.on("/", _handleRoot);
    _server.on("/save", HTTP_POST, _handleSave);
    
    // Captive portal redirects for iOS/Android/macOS
    _server.on("/generate_204", _handleRoot);
    _server.on("/redirect", _handleRoot);
    _server.on("/hotspot-detect.html", _handleRoot);
    _server.onNotFound(_handleRoot);

    _server.begin();
    Serial.println("[NetManager] HTTP Web Server and Captive Portal DNS running.");
}

inline void begin() {
    Serial.println("[NetManager] Initializing Wi-Fi...");
    _prefs.begin("wifi-config", false); // read-write mode to avoid NOT_FOUND error on first boot
    String ssid = _prefs.getString("ssid", "");
    String pass = _prefs.getString("pass", "");
    _prefs.end();

    // Fallback to config.h if valid
    if (ssid.isEmpty() && String(WIFI_SSID) != "your-wifi-name") {
        ssid = WIFI_SSID;
        pass = WIFI_PASSWORD;
        Serial.println("[NetManager] Using fallback Wi-Fi from config.h");
    }

    if (!ssid.isEmpty()) {
        Serial.print("[NetManager] Trying to connect to SSID: ");
        Serial.println(ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), pass.c_str());
        _lastAttemptMs = millis();
        
        // Wait up to 8s for Wi-Fi connection
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start) < 8000) {
            delay(200);
            Serial.print(".");
        }
        Serial.println();
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("[NetManager] WiFi Connected! IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        _startAP();
    }
}

inline bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

inline bool isApMode() {
    return _apMode;
}

inline void loop() {
    if (_apMode) {
        _dnsServer.processNextRequest();
        _server.handleClient();
        return;
    }

    if (isConnected()) return;

    uint32_t now = millis();
    if (now - _lastAttemptMs > 15000) {
        _lastAttemptMs = now;
        _prefs.begin("wifi-config", true);
        String ssid = _prefs.getString("ssid", "");
        String pass = _prefs.getString("pass", "");
        _prefs.end();
        if (!ssid.isEmpty()) {
            WiFi.disconnect();
            WiFi.begin(ssid.c_str(), pass.c_str());
        }
    }
}

} // namespace NetManager
