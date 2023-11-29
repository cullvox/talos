#pragma once

#include <AsyncWebSocket.h>

namespace ts {

struct SpotifyPlayerStatus
{
    bool nowPlaying;
    String title;
    String artist;
};

class Spotify {
public:
    void begin(AsyncWebServer& server);
    void end();

    bool getPlayerStatus(SpotifyPlayerStatus& status);
    
private:
    static void login(AsyncWebServerRequest* request);
    static void callback(AsyncWebServerRequest* request);

    void sendRequest();
    void requestRefreshToken();

    AsyncWebServer* _server;
    String _authCode;
    char _refreshToken[150];
};


} /* namespace ts */