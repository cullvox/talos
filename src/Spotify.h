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
    void addServerCallbacks(AsyncWebServer& server);
    String generateAuthLink();
    
private:
    void sendRequest();
    void requestRefreshToken();

    String _authCode;
    char _refreshToken[150];
};


} /* namespace ts */