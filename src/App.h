#pragma once

#include <Preferences.h>
#include <AsyncWebSocket.h>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Spotify.h"
#include "slides/SlideError.h"

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
    void displayError(Strings::Select primary, Strings::Select secondary);

    Preferences _prefs;
    BitmapAlloc _buffer;
    Render _render;
    Display _display;
    SlideError _slideGeneral;
    AsyncWebServer _server;
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