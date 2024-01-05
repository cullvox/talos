#pragma once

#include "Widget.h"

namespace ts {

class WidgetLastFM : public Widget {
public:
    virtual Extent2i getExtent() const override { return { 700, 300 }; }
    virtual bool fetch(WiFiClientSecure& client) override;
    virtual void render(Render& render) override;

private:
    String _artist;
    String _album;
    String _track;
    bool _currentlyListening;
    
};

} /* namespace ts */