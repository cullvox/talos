#pragma once

#include "Widget.h"

namespace ts {

class WidgetDigitalClock : public Widget {
public:
    virtual void render(Render& render) override;
    virtual bool fetch(WiFiClientSecure& client) override;
    virtual Extent2i getExtent() const override { return Extent2i{}; }

protected:
    char _time[32+1] = {};
};

} /* namespace ts */