#pragma once

#include <OpenFontRender.h>

#include "Bitmap.h"
#include "FlashStrings.h"

namespace ts {

/** @brief Aligns the cursor as this axis point for where to draw.
 *       The default value is eTopLeft.
 */
enum class RenderAlign {
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
enum class Color : uint16_t {
    eWhite = 0xFFFF,
    eBlack = 0x0000,
};

inline bool toBool(Color color) { 
    switch (color) {
        case Color::eBlack: return true;
        case Color::eWhite: return false;
        default: return false;
    }
}

/** @brief General purpose renderer for bit-per-pixel bitmap images. */
class Render {
public:
    Render();

    Render& setBitmap(BitmapInterface& bitmap);
    Render& setCursor(Vector2i pos);
    Render& setAlignment(RenderAlign align);
    Render& setFillColor(Color color);
    Render& setOutlineColor(Color color);
    Render& setOutlineThickness(uint16_t px);
    Render& setFontSize(uint16_t px);

    Vector2i getCursor();
    RenderAlign getAlign();
    Color getFillColor();
    Color getOutlineColor();
    uint16_t getOutlineThickness();
    BitmapInterface* getBitmap();

    Render& seekCursor(Vector2i deltaPos);

    Render& setPixel(Vector2i pos, Color color);
    Render& drawHLine(int x, int y, int length);
    Render& drawHLine(int x, int y, int length);
    Render& drawLine(Rect2i rect);
    Render& drawRect(Rect2i rect);
    Render& drawRoundedRect(Extent2i rect, int radius);
    Render& drawCircle(uint16_t radius);
    Render& drawArc(float cx, float cy, float px, float py, float theta, int N);


    Render& drawText(const char* text);
    Render& drawTextFormat(const char* format, ...);
    Render& drawTextFromFlash(Strings::Select selection);


    //Render& drawJpeg(Vector2i pos, const uint8_t* data, size_t length);

    bool loadFont(const unsigned char* data, size_t size);
    bool loadFont(const char* path);
    void unloadFont();
    uint16_t calculateFitFontSize(uint16_t maxWidth, uint16_t maxHeight, const char* str);
    
    template<typename... Args>
    uint16_t calculateFitFontSizeFmt(uint16_t maxWidth, uint16_t maxHeight, const char* format, Args... args)
    {
        return _ofr.calculateFitFontSize(maxWidth, maxHeight, Layout::Horizontal, format, args...);
    }

    Rect2i calculateTextBox(Vector2i position, uint16_t fontSize, RenderAlign align, const char* str);

    template<typename... Args>
    Rect2i calculateTextBoxFmt(Vector2i position, uint16_t fontSize, RenderAlign align, const char* format, Args... args)
    {
        FT_BBox bbox = _ofr.calculateBoundingBoxFmt(position.x, position.y, Layout::Horizontal, format, args...);

        return Rect2i{
            Vector2i{(int16_t)bbox.xMin, (int16_t)bbox.yMin}, 
            Extent2i{(uint16_t)(bbox.xMax - bbox.xMin), (uint16_t)(bbox.yMax - bbox.yMin)}};
    }

private:
    void ofr_print(const char* str);
    void ofr_drawPixel(int32_t x, int32_t y, uint16_t c);
    inline Align ofr_alignConvert(RenderAlign align);

    OpenFontRender _ofr;
    BitmapInterface* _bitmap;
    RenderAlign _align; 
    Vector2i _cursor;
    Color _fill;
    Color _outline;
    uint16_t _thickness;
};

} /* namespace ts */
