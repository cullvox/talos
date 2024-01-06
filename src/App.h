#pragma once

#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <AsyncWebSocket.h>
#include <SpotifyESP.h>

#include "Config.h"
#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Pages/PageGeneral.h"

namespace ts {

class Talos {
public:
    Talos();

    bool init();
    bool run();
    void shutdown();

private:
    static void printBegin(const char* name);
    static void printEnd(const char* name);
    static void printStartup();
    void readConfig();
    void writeConfig();
    static void clearConfig();
    bool connectToWiFi();
    bool preformFirstTimeSetup();
    bool preformSpotifyAuthorization();
    bool refreshSpotify();

    /** @brief Clears the configuration and restarts the device. */
    static void interruptClear();

    void displayGeneral(Strings::Select severity, Strings::Select primary, Strings::Select secondary);

    bool _bHasPsram;
    bool _bReset;
    Preferences _prefs;
    Display _display;
    BitmapAlloc _buffer;
    Render _render;
    PageGeneral _slideGeneral;
    AsyncWebServer _server;
    WiFiClientSecure _wifiClient;
    HTTPClient _httpClient;
    SpotifyESP _spotify;
    Config _config;
};

} /* namespace ts */