#pragma once

#include "Widget.h"
#include "SpotifyESP.h"

namespace ts {

class WidgetSpotify : public Widget {
public:
    WidgetSpotify(WiFiClientSecure& wifiClient, SpotifyESP& _spotify);

    virtual bool fetch(Render& render) override;

    virtual void render(Render& render) override;

private:
    WiFiClientSecure& _wifiClient;
    SpotifyESP* _spotify;
    Strings::Select _currentlyPlaying;
    size_t _imageLength;    
    uint8_t* _image = nullptr;
    char _title[SPOTIFY_NAME_CHAR_LENGTH];
    char _artist[SPOTIFY_NAME_CHAR_LENGTH];
    
};
