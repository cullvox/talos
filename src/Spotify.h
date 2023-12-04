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
public:;
    bool isRefreshRequired();
    bool blockingUpdateRefreshToken();
    bool blockingRequestPlaybackState(SpotifyPlayerStatus& status);
    void addAuthCallbacks(AsyncWebServer& server);
    String generateAuthLink();
    void waitForAuthorization();
    
private:
    void onRequestSpotifyAuthentication(AsyncWebServerRequest*);
    void onRequestSpotifyCallback(AsyncWebServerRequest*);

    bool _refreshRequired;
    uint64_t _refreshTime;
    char _codeVerifier[44] = "";
    char _refreshToken[150] = "";
};


} /* namespace ts */