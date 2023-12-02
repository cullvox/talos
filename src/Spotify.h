#pragma once

#include <AsyncWebSocket.h>

#define TS_SPOTIFY_CLIENT_ID "bb15029eaeea4bd086008584ae1fbaf7"

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