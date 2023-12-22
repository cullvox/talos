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
    size_t _imageLength;    
    uint8_t* _image = nullptr;
    char _title[SPOTIFY_NAME_CHAR_LENGTH];
    char _artist[SPOTIFY_NAME_CHAR_LENGTH];
    
};

}