#pragma once

#include "Slide.h"
#include "SpotifyESP.h"

namespace ts {

class SlideSpotify : public Slide {
public:
    SlideSpotify(WiFiClientSecure& wifiClient, SpotifyESP& _spotify, char* imageBuffer);

    virtual const char* name() { return "SpotifySlide"; }
    virtual bool fetch(Render& render) override;
    // virtual void overlay(Render& render) override;
    virtual void render(Render& render) override;

private:
    WiFiClientSecure& _wifiClient;
    SpotifyESP& _spotify;
    char* imageBuffer;
    Strings::Select _currentlyPlaying;    
    uint8_t* buffer = nullptr;
    int bufferSize = 0;
    char _title[SPOTIFY_NAME_CHAR_LENGTH];
    char _artist[SPOTIFY_NAME_CHAR_LENGTH];
    
};

}