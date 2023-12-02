#pragma once

#include <Preferences.h>
#include <AsyncWebSocket.h>
#include <ESPAsync_WiFiManager.hpp>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Spotify.h"
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
    static void generateSpotifyCodeVerifier(char* codeVerifier, uint32_t codeLength);

    Preferences _prefs;
    BitmapAlloc _buffer;
    Render _render;
    Display _display;
    SlideGeneral _slideGeneral;
    AsyncWebServer _server;
    AsyncDNSServer _dnsServer;
    // ESPAsync_WiFiManager _wifiManager;
    Spotify _spotify;

    struct Config {
        bool isFirstTimeSetup;
        char wifiSSID[64+1];
        char wifiPassword[32+1];
        bool spotifyEnabled;
        bool spotifyAuthorized;
    };

    Config _config;
};

} /* namespace ts */