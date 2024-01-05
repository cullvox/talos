#include "PageGeneral.h"

namespace ts {

template <typename T> T CLAMP(const T& value, const T& low, const T& high) 
{
    return value < low ? low : (value > high ? high : value); 
}

void SlideGeneral::render(Render& render)
{
    /* draw title */
    render
        .setAlignment(RenderAlign::eTopLeft)
        .setOutlineColor(Color::eWhite)
        .setFillColor(Color::eBlack)
        .setCursor(Vector2i{10, 10})
        .setFontSize(145)
        .drawTextFromFlash(_severity);

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

void SlideGeneral::setSeverity(Strings::Select selection)
{
    _severity = selection;
}

void SlideGeneral::setPrimary(Strings::Select selection) 
{ 
    Strings::copyTo(selection, _primary, sizeof(_primary)-1); 
}

void SlideGeneral::setSecondary(Strings::Select selection) 
{
    Strings::copyTo(selection, _secondary, sizeof(_secondary)-1); 
}

} /* namespace ts */