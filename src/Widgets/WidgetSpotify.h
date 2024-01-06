#pragma once

#include "Widget.h"
#include "SpotifyESP.h"

namespace ts {

class WidgetSpotify : public Widget {
public:
    WidgetSpotify(SpotifyESP& spotify);

    virtual Extent2i getExtent() const override { return { 750, 300 }; }
    virtual bool fetch(WiFiClientSecure& client) override;
    virtual void render(Render& render) override;

private:
    SpotifyESP* _spotify;
    Strings::Select _currentlyPlaying;
    size_t _imageLength;    
    uint8_t* _image = nullptr;
    char _title[SPOTIFY_NAME_CHAR_LENGTH];
    char _artist[SPOTIFY_NAME_CHAR_LENGTH];
};

} /* namespace ts */
