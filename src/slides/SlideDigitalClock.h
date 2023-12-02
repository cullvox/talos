#pragma once

#include "Slide.h"

namespace ts {

class SlideDigitalClock : public Slide {
public:
    virtual const char* name() override { return "DigitalClock"; }
    virtual bool fetch(Render& render) override;
    virtual void render(Render& render) override;

protected:
    char _time[32+1] = {};
};

} /* namespace ts */