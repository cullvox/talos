#pragma once

#include <OpenFontRender.h>
#include "Bitmap.h"

namespace ts {

class Slide {
public:
    virtual bool fetch(BitmapInterface* pBitmap) = 0;
    virtual void render(OpenFontRender& ofr) = 0;
};

} /* namespace ts */