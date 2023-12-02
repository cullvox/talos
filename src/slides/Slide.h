#pragma once

#include <Preferences.h>

#include "Render.h"

namespace ts {

class Slide {
public:
    virtual const char* name() { return "DefaultSlide"; }
    virtual bool fetch(Render& render) { return true; }
    virtual void overlay(Render& render) { }
    virtual void render(Render& render) { }
    virtual void serialize(Preferences& prefs) { }
    virtual void deserialize(Preferences& prefs) { }
};

} /* namespace ts */