#include "SlideError.h"

namespace ts {

template <typename T> T CLAMP(const T& value, const T& low, const T& high) 
{
    return value < low ? low : (value > high ? high : value); 
}

void SlideError::render(Render& render)
{
    /* draw title */
    render
        .setAlignment(RenderAlign::eTopLeft)
        .setOutlineColor(Color::eWhite)
        .setFillColor(Color::eBlack)
        .setCursor(Vector2i{10, 10})
        .setFontSize(145)

        .drawTextFromFlash(Strings::eError);

    /* draw the large data */
    
    unsigned int topSize = render.calculateFitFontSize(700, 200, _primary);
    Rect2i bounds = render.calculateTextBox(Vector2i{50, 480/2}, topSize, RenderAlign::eMiddleLeft, _primary);

    render
        .setAlignment(RenderAlign::eTopLeft)
        .setCursor(Vector2i{10, 480/2})
        .setFontSize(topSize)
        .drawText(_primary);

    /* draw the secondary smaller text */
    unsigned int bottomSize = render.calculateFitFontSize(700, 100, _secondary);

    bottomSize = CLAMP(topSize * 0.5f, 45.f, (float)topSize); 
    render
        .setCursor(Vector2i{10, (int16_t)(480/2 + topSize)})
        .setFontSize(bottomSize)
        .drawText(_secondary);

}

void SlideError::setPrimary(Strings::Select selection) 
{ 
    Strings::copyTo(selection, _primary, sizeof(_primary)-1); 
}

void SlideError::setSecondary(Strings::Select selection) 
{
    Strings::copyTo(selection, _secondary, sizeof(_secondary)-1); 
}

} /* namespace ts */