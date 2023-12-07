#pragma once

#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <AsyncWebSocket.h>
#include <SpotifyArduino.h>
#include <AsyncTCP.h>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "slides/SlideGeneral.h"

namespace ts {

class App {
public:
    App();

    bool init();
    void run();
    void shutdown();

private:
    bool connectToWiFi();
    bool preformFirstTimeSetup();
    bool preformSpotifyAuthorization();
    void displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary);

    Preferences _prefs;
    Display _display;
    BitmapAlloc _buffer;
    Render _render;
    SlideGeneral _slideGeneral;
    AsyncWebServer _server;
    WiFiClientSecure _wifiClient;
    HTTPClient _httpClient;
    SpotifyArduino _spotify;

    struct Config {
        bool isFirstTimeSetup;
        bool isWifiEnterprise;
        char wifiSSID[64+1];
        char wifiUsername[64+1];
        char wifiIdentity[64+1];
        char wifiPassword[32+1];
        bool spotifyEnabled;
        bool spotifyAuthorized;
    };

    Config _config;
};

} /* namespace ts */