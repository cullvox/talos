#include "SlideTalos.h"
#include "FlashStrings.h"

namespace ts {

void SlideTalos::render(Render& render)
{
    render
        .setCursor(Vector2i{50, 130})
        .setAlignment(RenderAlign::eBottomLeft)
        .setFontSize(200)
        .setFillColor(Color::eBlack)
        .setOutlineColor(Color::eWhite)
        .drawTextFromFlash(Strings::eTitle)

        .setCursor(Vector2i{50, 325})
        .setFontSize(45)
        .drawTextFromFlash(Strings::eTitleFull)

        .setCursor(Vector2i{50, 370})
        .setFontSize(50)
        .drawTextFromFlash(Strings::eCredit)

        .setCursor(Vector2i{790, 180})
        .setAlignment(RenderAlign::eBottomRight)
        .setFontSize(45)
        .drawTextFromFlash(Strings::eVersion);
}

} /* namespace ts */
