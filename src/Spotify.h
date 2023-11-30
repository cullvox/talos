#pragma once

#include <atomic>
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
    bool blockingRequestUserAuthorization();
    bool blockingRequestPlaybackState(SpotifyPlayerStatus& status);
    
private:
    static void login(AsyncWebServerRequest* request);
    static void callback(AsyncWebServerRequest* request);

    void sendRequest();
    void requestRefreshToken();

    String _authCode;
    char _refreshToken[150];
};


} /* namespace ts */