#pragma once

#include "Slide.h"

namespace ts {

class SlideTalos : public Slide {
public:
    virtual bool fetch(BitmapInterface* pBitmap) override;
    virtual void render(OpenFontRender& ofr) override;
};

} /* namespace ts */
