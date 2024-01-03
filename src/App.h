#pragma once

#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <AsyncWebSocket.h>
#include <SpotifyESP.h>

#include "Config.h"
#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Pages/Sandbox.h"

namespace ts {

class Talos {
public:
    Talos();

    bool init();
    bool run();
    void shutdown();

private:
    void printBegin(const char* name);
    void printEnd(const char* name);
    void printStartup();
    void readConfig();
    void writeConfig();
    bool connectToWiFi();
    bool preformFirstTimeSetup();
    bool preformSpotifyAuthorization();
    bool refreshSpotify();
    
    void displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary);

    bool _hasPsram;
    Preferences _prefs;
    Display _display;
    BitmapAlloc _buffer;
    Render _render;
    PageSandbox _slideGeneral;
    AsyncWebServer _server;
    WiFiClientSecure _wifiClient;
    HTTPClient _httpClient;
    SpotifyESP _spotify;
    Config _config;
};

} /* namespace ts */