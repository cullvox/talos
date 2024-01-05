#pragma once

#include "Widget.h"

namespace ts {

class WidgetDigitalClock : public Widget {
public:
    virtual bool fetch(WiFiClientSecure& client) override;
    virtual void render(Render& render) override;

protected:
    char _time[32+1] = {};
};

} /* namespace ts */