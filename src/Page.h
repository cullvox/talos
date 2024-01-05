#pragma once

#include <WiFiClientSecure.h>
#include "Render.h"

/** @brief A page is a collection of widgets rendered. */
class Page {
public:


    /** @brief Draws the page to the screen using a render device.
     * 
     * Overload this function to render your page to the screen. This function
     * is always called after fetch to ensure the most up-to-date info for the 
     * page. Considering that you probably want text, icons and other beautiful
     * things on screen try to ensure you do most of the hard work in the fetch
     * function for a speedy and straight forward render. This function is 
     * required to be implemented.
     * 
     * @param[in] render The render device to use when drawing.
     * 
     */
    virtual void render(Render& render) = 0;

    /** @brief Gathers data from the internet and other sources and preparing 
     * data for render.
     * 
     * Overload this function for gathering drawing data bout what ever you 
     * might want to render. This method will be called just before rendering
     * and will never be called directly before or after another page's fetch.
     * Try to ensure that this functions is speedy as rendering will likely 
     * take just as long as the fetch. 
     * 
     * @param client A WiFi client to change the settings of when fetching from the internet.
     * 
     * @return bool Returns true on successful fetch, otherwise the page will not be rendered and will be skipped.
     * 
     */
    virtual bool fetch(WiFiClientSecure& client) 
        { return true; }

};
