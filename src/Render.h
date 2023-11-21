#pragma once

#include <OpenFontRender.h>

#include "Bitmap.h"
#include "FlashStrings.h"

namespace ts {

/** @brief Aligns the cursor as this axis point for where to draw.
 *       The default value is eTopLeft.
 */
enum class RenderAlignment {
    eTopLeft,
    eTopCenter,
    eTopRight,
    
    eMiddleLeft,
    eMiddleCenter,
    eMiddleRight,

    eBottomLeft,
    eBottomCenter,
    eBottomRight,

    eCenter = eMiddleCenter
};

/** @brief Colors capable of drawing with. */
enum class Color : uint8_t {
    eWhite = 0xFF,
    eBlack = 0x00,
};

/** @brief General purpose renderer for bit-per-pixel bitmap images. */
class Render {
public:

    Render& setBitmap(BitmapInterface& pBitmap);
    Render& setCursor(Vector2i position);
    Render& setAlignment(RenderAlignment alignment);
    Render& setFillColor(Color color);
    Render& setOutlineColor(Color color);
    Render& setOutlineThickness(uint16_t px);

    Render& drawLine(Rect2i rect);
    Render& drawRect(Rect2i rect);
    Render& drawCircle(uint16_t radius);
    Render& drawText(const char* text);
    Render& drawTextFormat(const char* format, ...);
    Render& drawTextFromFlash(Strings::Select selection);


private:
    OpenFontRender _ofr;
    BitmapInterface* _bitmap;
    RenderAlignment _alignment; 
    Vector2i _cursor;
    Color _fill;
    Color _outline;
};

} /* namespace ts */
