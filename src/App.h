#pragma once

#include <Preferences.h>
#include <AsyncWebSocket.h>

#include "Bitmap.h"
#include "Render.h"
#include "Display.h"
#include "Spotify.h"

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
    bool preformFirstTimeSetup();
    static void setupCallback(AsyncWebServerRequest*);

    Preferences _prefs;
    BitmapAlloc _buffer;
    Render _render;
    Display _display;
    State _state;
    AsyncWebServer _server;
    Spotify _spotify;
    
};

} /* namespace ts */