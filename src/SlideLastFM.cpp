#include <Arduino.h>
#include <ArduinoJson.h>
#include <StreamString.h>
#include <HTTPClient.h>
#include <TJpg_Decoder.h>

#include "SlideLastFM.h"
#include "Print.h"
#include "ImageDraw.h"
#include "Secrets.h"

int JPEGDraw(JPEGDRAW *pDraw)
{
    bool dither = true;
    int16_t x = pDraw->x;
    int16_t y = pDraw->y;
    int16_t w = pDraw->iWidth;
    int16_t h = pDraw->iHeight;

    for(int i = 0; i < w * h; i++)
    {
        pDraw->pPixels[i] = (pDraw->pPixels[i] & 0x7e0) >> 5; // extract just the six green channel bits.
    }

    if (dither)
    {
        for(int16_t j = 0; j < h; j++)
        {
            for(int16_t i = 0; i < w; i++)
            {
                int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
                int8_t newPixel = oldPixel & 0x38; // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
                pDraw->pPixels[i + j * w] = newPixel;
                int quantError = oldPixel - newPixel;      
                if(i + 1 < w) pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
                if((i - 1 >= 0) && (j + 1 < h)) pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
                if(j + 1 < h) pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
                if((i + 1 < w) && (j + 1 < h)) pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
            } // for i
        } // for j
    } // if dither
  
    for(int16_t i = 0; i < w; i++)
    {
        for(int16_t j = 0; j < h; j++)
        {
            ts::Vector2i pos{(int16_t)(x+i), (int16_t)(y+j)}; 
            ts::BitmapInterface* pBuffer = (ts::BitmapInterface*)pDraw->pUser;

            switch (constrain(pDraw->pPixels[i + j * w] >> 4, 0, 3))
            {
                case 0:
                case 1:
                pBuffer->set(pos, false);
                break;
                case 2:
                case 3:
                    pBuffer->set(pos, true);
                break;
            } // switch
        } // for j
    } // for i
    return 1;
} /* JPEGDraw() */

namespace ts {




bool SlideLastFM::fetch(BitmapInterface* pBitmap)
{
    TS_INFO("Fetching LastFM HTTP GET Request\n");

    #define TALOS_LASTFM_URL "http://ws.audioscrobbler.com/2.0/?method=user.getrecenttracks&limit=1&format=json&user=" TS_SECRET_LASTFM_USER "&api_key=" TS_SECRET_LASTFM_KEY

    HTTPClient http;
    if (!http.begin(TALOS_LASTFM_URL))
    {
        TS_ERROR("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    // Send HTTP GET request
    String payload = "{}";
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        TS_INFOF("HTTP Response code: %d\n", httpResponseCode);
        payload = http.getString();
        TS_INFOF("%s\n", payload.c_str());
    }
    else {
        TS_ERRORF("Error code: %d\n", httpResponseCode);
        return false;
    }
    
    // Free resources
    http.end();

    StaticJsonDocument<1024> json{};
    deserializeJson(json, payload);

    _artist = json["recenttracks"]["track"][0]["artist"]["#text"].as<String>();
    _album = json["recenttracks"]["track"][0]["album"]["#text"].as<String>();
    _track = json["recenttracks"]["track"][0]["name"].as<String>();
    String imageURL = json["recenttracks"]["track"][0]["image"][2]["#text"].as<String>();

    TS_INFOF("Latest Artist: %s", _artist.c_str());
    TS_INFOF("Latest Album: %s", _album.c_str());
    TS_INFOF("Latest Tracks: %s", _track.c_str());

    // if (_track.length() > 16)
    // {
    //     _track = _track.substring(0, 16);
    //     _track.concat("...");
    // }

    String nowplaying = json["recenttracks"]["track"][0]["@attr"]["nowplaying"].as<String>();
    nowplaying.toLowerCase();

    _currentlyListening = nowplaying == "true" ? true : false;

    imageURL = "http" + imageURL.substring(5);

    if (!http.begin(imageURL))
    {
        TS_ERROR("http Failed!\n");
        return false;
    }

    http.addHeader("Connection", "close");

    // Send HTTP GET request
    httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
        printf("HTTP Response code: %d\n", httpResponseCode);
        
        payload = http.getString();
        
        TS_ERRORF("%s\n", payload.c_str());
    }
    else {
        printf("Error code: %d", httpResponseCode);
        return false;
    }
    
    // Free resources
    http.end();


    JPEGDEC* dec = new JPEGDEC();
    dec->setUserPointer(pBitmap);
    dec->setPixelType(ONE_BIT_DITHERED);

    if (!dec->openRAM((uint8_t*)payload.c_str(), payload.length(), JPEGDraw))
    {
        TS_ERROR("Could not open JPEGDEC from memory!\n");
    }    

    if (!dec->decode(0, 0, 0))
    {
        TS_ERROR("Could not decode JPEGDEC image!");
    }

    return true;
}

void SlideLastFM::render(OpenFontRender& ofr)
{
    unsigned int trackSize = ofr.calculateFitFontSize(700, 150, Layout::Horizontal, _track.c_str());
    FT_BBox trackBounds = ofr.calculateBoundingBox(800/2, 480/2, trackSize, Align::BottomCenter, Layout::Horizontal, _track.c_str());
    
    ofr.setFontColor(0x0000);
    ofr.setBackgroundColor(0xFFFF);
    ofr.setFontSize(trackSize);
    ofr.setAlignment(Align::BottomCenter);
    ofr.setCursor(800/2, 480/2);

    ofr.printf("%s", _track.c_str());


    const char* pPlayingText = _currentlyListening ? "currently playing" : "was playing";
    FT_BBox playingBounds = ofr.calculateBoundingBox(trackBounds.xMin, trackBounds.yMin, 96, Align::BottomLeft, Layout::Horizontal, pPlayingText);

    ofr.setAlignment(Align::BottomLeft);
    ofr.setCursor(trackBounds.xMin, playingBounds.yMin); // 480/2 + 1 - trackSize/2
    ofr.setFontSize(96);

    ofr.printf("%s", pPlayingText);

    ofr.setAlignment(Align::TopRight);
    ofr.setCursor(trackBounds.xMax, trackBounds.yMax);

    ofr.printf("%s", _artist.c_str());
    

}

} /* namespace ts */
