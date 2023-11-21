#pragma once


#include "JPEGDEC.h"

#include "Slide.h"


namespace ts {

class SlideLastFM : public Slide {
public: 
    virtual bool fetch(BitmapInterface* pBitmap) override;
    virtual void render(OpenFontRender& ofr) override;

private:
    String _artist;
    String _album;
    String _track;
    bool _currentlyListening;
    
};

} /* namespace ts */