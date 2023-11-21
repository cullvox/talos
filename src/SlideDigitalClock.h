#pragma once

#include "Slide.h"

namespace ts {

class SlideDigitalClock : public Slide {
public:
    virtual bool fetch(BitmapInterface* pBitmap) override;
    virtual void render(OpenFontRender& ofr) override;

protected:
    char _time[32] = {};
};

} /* namespace ts */