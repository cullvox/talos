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
    static void printStartup(); /* prints the talos text (for pre initialization). */
    static void printBegin(const char* name); /* prints the context begin text. */
    static void printEnd(const char* name); /* prints the context end text. */
    static void interruptClear(void* arg); /* callback called clear config button is pressed. */

    bool connectToWiFi(); /* connects the esp32 to wifi.*/
    bool preformFirstTimeSetup(); /* clears the config and does all fts. */
    bool preformSpotifyAuthorization(); /* authorizes the user with spotify if they wanted. */
    bool refreshSpotify(); /* recreates the spotify refresh token. */

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