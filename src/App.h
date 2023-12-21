#pragma once

#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <AsyncWebSocket.h>
#include <SpotifyESP.h>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "slides/SlideGeneral.h"

namespace ts {

class App {
public:
    App();

    bool init();
    bool run();
    void shutdown();

private:
    void printStartup();
    bool connectToWiFi();
    bool preformFirstTimeSetup();
    bool preformSpotifyAuthorization();
    bool refreshSpotify();
    
    void displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary);


    Preferences _prefs;
    Display _display;
    BitmapAlloc _buffer;
    Render _render;
    SlideGeneral _slideGeneral;
    AsyncWebServer _server;
    WiFiClientSecure _wifiClient;
    HTTPClient _httpClient;
    SpotifyESP _spotify;
    char* spotifyImageBuffer;

    struct Config {
        bool isFirstTimeSetup;
        bool isWifiEnterprise;
        char wifiSSID[64+1];
        char wifiUsername[64+1];
        char wifiIdentity[64+1];
        char wifiPassword[32+1];
        bool spotifyEnabled;
        bool spotifyAuthorized;
        String spotifyRefreshToken; 
    };

    Config _config;
};

} /* namespace ts */