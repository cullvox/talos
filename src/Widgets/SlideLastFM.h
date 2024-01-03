#pragma once


#include "JPEGDEC.h"

#include "Widget.h"


namespace ts {

class SlideLastFM : public Slide {
public: 
    virtual bool fetch(Render&) override;
    virtual void render(Render&) override;

private:
    String _artist;
    String _album;
    String _track;
    bool _currentlyListening;
    
};

} /* namespace ts */