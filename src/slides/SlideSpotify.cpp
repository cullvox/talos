#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Secrets.h"
#include "SlideSpotify.h"

namespace ts {

bool SlideSpotify::fetch(Render& render)
{

    /* Spotifys API requires us to generate a token before other requests. */
    TS_INFO("Requesting Spotify access token\n");
    
    HTTPClient http;
    if (!http.begin("https://accounts.spotify.com/api/token"))
    {
        TS_ERROR("Could not begin HTTP client for Spotify token request!\n");
        return false;
    }

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpCode = 0;
    httpCode = http.POST("grant_type=client_credentials&client_id=" TS_SECRET_SPOTIFY_CLIENT_ID "&client_secret=" TS_SECRET_SPOTIFY_CLIENT_SECRET);

    String payload = http.getString();
    http.end();

    if (httpCode < 0) 
    {
        TS_ERROR("Request failed for Spotify access token.\n");
        return false; 
    }    

    TS_INFOF("Spotify Payload: %s", payload.c_str());

    /* Parse the JSON payload for an access token. */
    String accessToken = "";
    
    {
        StaticJsonDocument<256> json;
        deserializeJson(json, payload);

        accessToken = "Bearer " + json["access_token"].as<String>();
    }

    TS_INFOF("Recieved Spotify access token: %s\n", accessToken.c_str());

    /* Request the users playback information. */
    http.begin("https://api.spotify.com/v1/me/player");
    http.addHeader("Authorization", accessToken);
    
    httpCode = http.GET();

    payload = http.getString();
    http.end();

    if (httpCode < 0) 
    {
        TS_ERROR("Request failed for Spotify -> get playback state.");
        return false; 
    }

    TS_INFOF("Spotify Payload: %s", payload.c_str());

    {
        StaticJsonDocument<1024> json;
        deserializeJson(json, payload);

        _artist = json["item"]["artists"][0]["name"].as<String>();
        _title = json["item"]["name"].as<String>();
        _currentlyPlaying = json["is_playing"].as<bool>() ? Strings::eNowPlaying : Strings::eWasPlaying;
    }



    // imageURL = "http" + imageURL.substring(5);
// 
    // if (!http.begin(imageURL))
    // {
    //     TS_ERROR("http Failed!\n");
    //     return false;
    // }
// 
    // http.addHeader("Connection", "close");
// 
    // // Send HTTP GET request
    // httpResponseCode = http.GET();
    // 
    // if (httpResponseCode > 0) {
    //     printf("HTTP Response code: %d\n", httpResponseCode);
    //     
    //     payload = http.getString();
    //     
    //     TS_ERRORF("%s\n", payload.c_str());
    // }
    // else {
    //     printf("Error code: %d", httpResponseCode);
    //     return false;
    // }
    // 
    // // Free resources
    // http.end();
// 
// 
    // TS_INFO("Creting a JPEG description!\n");
    // JPEGDEC* dec = new JPEGDEC();
// 
    // TS_INFOF("Bitmap: %p\n", render.getBitmap());
// 
    // dec->setUserPointer(render.getBitmap());
    // dec->setPixelType(ONE_BIT_DITHERED);
// 
    // TS_INFO("Opening JPEG flash\n");
    // if (!dec->openRAM((uint8_t*)payload.c_str(), payload.length(), JPEGDraw))
    // {
    //     TS_ERROR("Could not open JPEGDEC from memory!\n");
    // }    
// 
    // dec->setUserPointer(render.getBitmap());
// 
    // TS_INFO("Decoding jpeg image!\n");
    // if (!dec->decode(0, 0, 0))
    // {
    //     TS_ERROR("Could not decode JPEGDEC image!\n");
    // }
// 
    // TS_INFO("Decoded JPEG image!\n");

    return true;
}

void SlideSpotify::render(Render& render)
{
    const int16_t titleY = 480/2;
    const int16_t largeFont = 96;
    const int16_t smallFont = 56;

    render
        .setAlignment(RenderAlign::eBottomLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY - largeFont/2)}) // 480/2 + 1 - trackSize/2
        .setFontSize(smallFont)
        .drawTextFromFlash(_currentlyPlaying)

        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .setFontSize(96)
        .setAlignment(RenderAlign::eMiddleLeft)
        .setCursor(Vector2i{10, titleY})
        .drawText(_title.c_str())

        .setAlignment(RenderAlign::eTopLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY + largeFont/2)})
        .drawText(_artist.c_str()); 

}

} /* namespace ts */