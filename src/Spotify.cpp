#include <ESPmDNS.h>

#include "Config.h"
#include "Spotify.h"

namespace ts {

bool Spotify::blockingRequestUserAuthorization()
{

    TS_INFO("Beginning Spotify authentication process.\n");

    /* Add spotify callbacks for the authentication process. */
    std::atomic<bool> finished;

    /* Start MDNS for to use talos.local for Spotify config. */
    TS_INFO("Beginning MDNS.\n");
    MDNS.begin("talos");

    TS_INFO("Beginning Web Server.\n");
    AsyncWebServer server(80);

    server.on("/spotify", HTTP_GET, [](AsyncWebServerRequest* request){
        /* Generate a random state value. */
        const int stateLength = 16;
        char state[stateLength+1];
        memset(state, 0, sizeof(state));
        esp_fill_random(state, sizeof(state)-1);

        /* Spotify scope of TALOS. */
        const __FlashStringHelper* scope = 
        F("user-read-private+"
            "user-read-currently-playing+"
            "user-read-playback-state");

        /* Redirect Spotify's API to the spotify_callback to receive the code. */
        const __FlashStringHelper* redirect = F("http://talos.local/spotify_callback");

        /* Build the Spotify redirect for authorization. */
        String url;
        url.reserve(400);
        url.concat(F("https://accounts.spotify.com/authorize/?response_type=code&scope="));
        url.concat(scope);
        url.concat(F("&redirect_uri="));
        url.concat(redirect);
        url.concat(F("&state="));
        url.concat(state);

        request->redirect(url);
    });

    server.on("/spotify_callback", HTTP_POST, [&finished](AsyncWebServerRequest* request){

        /* Retrieve the spotify access code. */
        AsyncWebParameter* code = request->getParam("code");
        if (!code)
        {
            TS_ERROR("Could not find Spotify access code in callback!");
            return;
        }
        
        String spotifyCode = code->value();
        TS_INFOF("Recieved Spotify authorization code: %s\n", spotifyCode.c_str());

        finished = true;
    });

    server.begin();

    /* Wait until authentication is complete. */
    while (!finished) {}

    /* Cleanup server stuffs. */
    server.end();
    MDNS.end();

}

bool Spotify::blockingRequestPlaybackState(SpotifyPlayerStatus& status)
{

}


void Spotify::login(AsyncWebServerRequest* request)
{


}

void Spotify::callback(AsyncWebServerRequest* request)
{
    


}

} /* namespace ts */