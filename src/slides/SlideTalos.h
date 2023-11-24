#pragma once

#include "Slide.h"

namespace ts {

class SlideTalos : public Slide {
public:
    virtual void render(Render&) override;
};

} /* namespace ts */
