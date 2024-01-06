#include <Arduino.h>
#include <ArduinoJson.h>
#include <StreamString.h>
#include <HTTPClient.h>
#include <JPEGDEC.h>

#include "WidgetLastFM.h"

namespace ts {

static const uint8_t bayerThresholdMap [4][4] = {
    {15, 135, 45, 165},
    {195, 75, 225, 105},
    {60, 180, 30, 150},
    {240, 120, 210, 90},
};

static int JPEGDraw(JPEGDRAW *pDraw)
{
    log_i("Drawing image!\n");

    bool dither = true;
    int16_t x = pDraw->x;
    int16_t y = pDraw->y;
    int16_t w = pDraw->iWidth;
    int16_t h = pDraw->iHeight;

    ts::BitmapInterface* pBuffer = (ts::BitmapInterface*)pDraw->pUser;
    uint8_t threshold = 128;

    for(int16_t j = 0; j < h; j++)
    {
        for(int16_t i = 0; i < w; i++)
        {
            pDraw->pPixels[i + j * w] = (pDraw->pPixels[i + j * w] & 0x7e0) >> 5; // extract just the six green channel bits.
        
            // Bayer Dithering
            // 4x4 Bayer ordered dithering algorithm
            const uint16_t bayerx = (i + j * w) % w;
            const uint16_t bayery = floor((i + j * w) / w);
            const uint16_t bayerMapped = 0 + (pDraw->pPixels[i + j * w] - 0) * (255 - 0) / (63 - 0);
            const uint16_t map = floor((bayerMapped + bayerThresholdMap[bayerx % 4][bayery % 4]) / 2);
            const ts::Vector2i pos{x+i, y+j};
            pBuffer->set(pos, (map < threshold) ? false : true);
        }
    }

    

    // 
    // if (dither)
    // {
    //     for(int16_t j = 0; j < h; j++)
    //     {
    //         for(int16_t i = 0; i < w; i++)
    //         {
    //             int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
    //             int8_t newPixel = oldPixel & 0x30; // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
    //             pDraw->pPixels[i + j * w] = newPixel;
    //             int quantError = oldPixel - newPixel;      
    //             if(i + 1 < w) 
    //                 pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
    //             if((i - 1 >= 0) && (j + 1 < h)) 
    //                 pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
    //             if(j + 1 < h) 
    //                 pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
    //             if((i + 1 < w) && (j + 1 < h)) 
    //                 pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
    //         } // for i
    //     } // for j
    // } // if dither
  // 
    // ts::BitmapInterface* pBuffer = (ts::BitmapInterface*)pDraw->pUser;
    // ts::Vector2i pos;
    // for(int16_t i = 0; i < w; i++)
    // {
    //     for(int16_t j = 0; j < h; j++)
    //     {
    //         pos = ts::Vector2i{(int16_t)(x+i), (int16_t)(y+j)};
    //         
    //         switch (constrain(pDraw->pPixels[i + j * w] >> 4, 0, 3))
    //         {
    //             
    //             case 0:
    //             pBuffer->set(pos, false);
    //             break;
    //             case 1:
    //             case 2:
    //             case 3:
    //                 pBuffer->set(pos, true);
    //             break;
    //         } // switch
    //     } // for j
    // } // for i
    return 1;
} /* JPEGDraw() */

bool WidgetLastFM::fetch(WiFiClientSecure& client)
{
    log_i("Fetching LastFM HTTP GET Request\n");

    // #define TALOS_LASTFM_URL "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&limit=1&format=json&user=" TS_SECRET_LASTFM_USER "&api_key=" TS_SECRET_LASTFM_KEY

    HTTPClient http;
    // if (!http.begin(TALOS_LASTFM_URL))
    {
        log_e("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    // Send HTTP GET request
    String payload = "{}";
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        log_i("HTTP Response code: %d\n", httpResponseCode);
        payload = http.getString();
        log_i("%s\n", payload.c_str());
    }
    else {
        log_e("Error code: %d\n", httpResponseCode);
        return false;
    }
    
    // Free resources
    http.end();

    StaticJsonDocument<1024> json{};
    deserializeJson(json, payload);

    _artist = json["recenttracks"]["track"][0]["artist"]["#text"].as<String>();
    _album = json["recenttracks"]["track"][0]["album"]["#text"].as<String>();
    _track = json["recenttracks"]["track"][0]["name"].as<String>();
    String imageURL = json["recenttracks"]["track"][0]["image"][1]["#text"].as<String>();

    log_i("Latest Artist: %s\n", _artist.c_str());
    log_i("Latest Album: %s\n", _album.c_str());
    log_i("Latest Tracks: %s\n", _track.c_str());

    if (_track.length() > 16)
    {
        _track = _track.substring(0, 16);
        _track.concat("...");
    }

    String nowplaying = json["recenttracks"]["track"][0]["@attr"]["nowplaying"].as<String>();
    nowplaying.toLowerCase();

    _currentlyListening = nowplaying == "true" ? true : false;

    return true;
}

void WidgetLastFM::render(Render& render)
{
    Strings::Select playingSelect = _currentlyListening ? Strings::ePlaybackNowPlaying : Strings::ePlaybackWasPlaying; 
    
    const int16_t titleY = 480/2;
    const int16_t largeFont = 96;
    const int16_t smallFont = 56;

    render
        .setAlignment(RenderAlign::eBottomLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY - largeFont/2)}) // 480/2 + 1 - trackSize/2
        .setFontSize(smallFont)
        .drawTextFromFlash(playingSelect)

        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .setFontSize(96)
        .setAlignment(RenderAlign::eMiddleLeft)
        .setCursor(Vector2i{10, titleY})
        .drawText(_track.c_str())

        .setAlignment(RenderAlign::eTopLeft)
        .setCursor(Vector2i{10, (int16_t)(titleY + largeFont/2)})
        .drawText(_artist.c_str()); 

}

} /* namespace ts */

