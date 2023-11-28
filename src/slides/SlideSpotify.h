#pragma once

#include "Slide.h"

namespace ts {

class SlideSpotify : public Slide {
public:
    virtual std::string_view name() { return "SpotifySlide"; }
    virtual bool fetch(Render& render) override;
    // virtual void overlay(Render& render) override;
    virtual void render(Render& render) override;

private:
    Strings::Select _currentlyPlaying;
    String _title;
    String _artist;
    
};

}