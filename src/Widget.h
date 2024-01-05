#pragma once

#include <WiFiClientSecure.h>

#include "Render.h"

namespace ts {

class Widget {
public:

    /** @brief Returns the size of the rendered widget. 
     * 
     * Widgets have a predefined maximum size and that can be found using this
     * function. Returns the size of the widget in pixels.
     * 
     */
    virtual Extent2i getExtent() const = 0;

    /** @brief Renders the widget using a renderer. 
     * 
     * Draws a widget to the display at a location using a render object.
     * This default function draws a rounded rectangle at a radius of five 
     * pixels. Its a small rounding just enough to be noticed on the screen
     * and appreciated.
     * 
     */
    virtual void render(Render& render);

    virtual bool fetch(WifiClientSecure& client) { return true; }
};

} /* namespace ts */
