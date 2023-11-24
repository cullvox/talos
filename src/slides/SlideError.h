#pragma once

#include "FlashStrings.h"
#include "Slide.h"

namespace ts {

class SlideError : public Slide {
public:
    virtual void render(Render& ofr) override;

    void setPrimary(Strings::Select index);
    void setSecondary(Strings::Select index);

private:
    char _primary[32+1];
    char _secondary[32+1];
};

} /* namespace ts */