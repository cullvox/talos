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
#include "Pages/Sandbox.h"

namespace ts {

class Talos {
public:
    Talos();

    /** @brief Initializes the device and ensures that we have initialized to a useful point. 
     * 
     * Initializes the device to be connected to the internet and prepared to 
     * display pages. Returns true even when WiFi connectivity fails so that
     * users with a poor or currently nonexistant internet connection can
     * ensure that their device works. Will display an error on the screen 
     * if possible when initialization effectively fails.
     * 
     * @return Anything other than true means that first time setup or before that failed. (serious problem)
     * 
     */
    bool init();

    
    void run();
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
    static void interruptClear(void* arg);

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

    PageSandbox pageSandbox;
};

} /* namespace ts */