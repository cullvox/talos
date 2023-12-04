#include <ESPmDNS.h>
#include <mbedtls/sha256.h>
#include <base64.h>
#include <ESPNtpClient.h>

#include "Config.h"
#include "Spotify.h"

namespace ts {

bool Spotify::isRefreshRequired()
{
    return (time(NULL) >= _refreshTime);
}

bool Spotify::blockingUpdateRefreshToken()
{
    return true;
}

bool Spotify::blockingRequestPlaybackState(SpotifyPlayerStatus& status)
{

}

void Spotify::addAuthCallbacks(AsyncWebServer& server)
{
    server.on("/spotify", HTTP_GET, std::bind(&Spotify::onRequestSpotifyAuthentication, this, std::placeholders::_1));
    server.on("/spotify_callback", HTTP_GET, std::bind(&Spotify::onRequestSpotifyCallback, this, std::placeholders::_1));
}

void Spotify::onRequestSpotifyAuthentication(AsyncWebServerRequest* request)
{
    /* Generate a random verifier value. */
    unsigned char verifier[32];
    for (int i = 0; i < 32; i++)
        verifier[i] = random(256);
    
    /* Encode the verifier to base64. */
    String encoded = base64::encode((const uint8_t*)verifier, 32);
    encoded.replace('+', '-');
    encoded.replace('/', '_');

    strncpy(_codeVerifier, encoded.c_str(), 43);

    unsigned char codeVerifierSha[32];
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, false);
    mbedtls_sha256_update(&ctx, (const unsigned char*)_codeVerifier, 32);
    mbedtls_sha256_finish(&ctx, verifier);
    mbedtls_sha256_free(&ctx);

    String codeVerifierShaBase64 = base64::encode((uint8_t*)codeVerifierSha, sizeof(codeVerifierSha));
    codeVerifierShaBase64.replace('+', '-');
    codeVerifierShaBase64.replace('/', '_');

    char spotifyCodeChallenge[44] = "";
    strncpy(spotifyCodeChallenge, codeVerifierShaBase64.c_str(), 43);

    log_i("Generated the Spotify code verification:\n\t");
    log_i("\tVerifier %.32s", verifier);
    log_i("\tVerifier (base64) %s", encoded.c_str());
    log_i("\tVerifier SHA256 %.32s", codeVerifierSha);
    log_i("\tCode Challenge %.32s", spotifyCodeChallenge);

    /* Build the Spotify authentication URL and redirect the user there. */
    String url;
    url.reserve(500);
    url.concat(
        F("https://accounts.spotify.com/authorize/?"
        "response_type=code"
        "&client_id=" TS_SPOTIFY_CLIENT_ID
        "&scope="
            "user-read-private+"
            "user-read-currently-playing+"
            "user-read-playback-state"
        "&redirect_uri=http%3A%2F%2Ftalos.local/spotify_callback"
        "&code_challenge_method=S256"
        "&code_challenge="));
    url.concat(spotifyCodeChallenge);

    log_i("Spotify authorization redirect generated: %s", url.c_str());

    request->redirect(url);
}

void Spotify::onRequestSpotifyCallback(AsyncWebServerRequest* request)
{
    log_i("Received callback from Spotify\n");

    /* Retrieve the spotify access code. */
    AsyncWebParameter* code = request->getParam("code");
    if (!code)
    {
        log_e("Could not find Spotify access code in callback!");
        return;
    }
    
    String spotifyCode = code->value();
    log_i("Received authorization code from Spotify: %s\n", spotifyCode.c_str());

}

} /* namespace ts */