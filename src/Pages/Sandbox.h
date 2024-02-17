#pragma once

#include "Page.h"

#include "Widgets/WidgetDigitalClock.h"
#include "Widgets/WidgetSpotify.h"

namespace ts {

class PageSandbox : public Page {
public:
    PageSandbox(SpotifyESP& _spotify);

    virtual void render(Render& render) override;
    virtual bool fetch(WiFiClientSecure& client) override;

    WidgetSpotify widSpotify;
    WidgetDigitalClock widClock;
};

} /* namespace ts */
