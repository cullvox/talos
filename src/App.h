#pragma once

#include <Preferences.h>
#include <AsyncWebSocket.h>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Spotify.h"
#include "slides/SlideError.h"

namespace ts {

enum class State {
    eFirstTimeSetup = -1,
    eInit,
    eRunning,
    eShutdown,
};

class App {
public:
    App();

    bool init();
    void run();
    void shutdown();

private:
    bool connectToWiFi();
    bool preformFirstTimeSetup();
    void displayError(Strings::Select primary, Strings::Select secondary);

    Preferences _prefs;
    BitmapAlloc _buffer;
    Render _render;
    Display _display;
    State _state;
    AsyncWebServer _server;
    bool _enableSpotify;
    Spotify _spotify;
    SlideError _slideError;

    char _wifiSSID[64+1];
    char _wifiPassword[32+1];
    
};

} /* namespace ts */