#include <cstdarg>

#include "Render.h"

namespace ts {

Render::Render()
{
    _ofr.set_printFunc(std::bind(&Render::ofr_print, this, std::placeholders::_1));
    _ofr.set_drawPixel(std::bind(&Render::ofr_drawPixel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // _ofr.setDebugLevel(OFR_DEBUG | OFR_ERROR | OFR_INFO);
    _ofr.setDebugLevel(OFR_ERROR | OFR_INFO);
    _ofr.showCredit();
    _ofr.showFreeTypeVersion();
}

Render& Render::setBitmap(BitmapInterface& bitmap)
{
    _bitmap = &bitmap;
    return *this;
}

Render& Render::setCursor(Vector2i pos)
{
    _cursor = pos;
    _ofr.setCursor((int32_t)pos.x, (int32_t)pos.y);
    return *this;
}

Render& Render::setAlignment(RenderAlign align)
{
    _align = align;
    _ofr.setAlignment(ofr_alignConvert(align));
    return *this;
}

Render& Render::setFillColor(Color color)
{
    _fill = color;
    _ofr.setFontColor((uint16_t)color);
    return *this;
}

Render& Render::setOutlineColor(Color color)
{
    _outline = color;
    _ofr.setBackgroundColor((uint16_t)color);
    return *this;
}

Render& Render::setOutlineThickness(uint16_t px)
{
    _thickness = px;
    return *this;
}

Render& Render::setFontSize(uint16_t px)
{
    _ofr.setFontSize(px);
    return *this;
}

Vector2i Render::getCursor()
{
    return _cursor;
}

RenderAlign Render::getAlign()
{
    return _align;
}

Color Render::getFillColor()
{
    return _fill;
}

Color Render::getOutlineColor()
{
    return _outline;
}

uint16_t Render::getOutlineThickness()
{
    return _thickness;
}

BitmapInterface* Render::getBitmap()
{
    return _bitmap;
}

Render& Render::seekCursor(Vector2i deltaPos)
{
    _cursor += deltaPos;
    _ofr.setCursor(_cursor.x, _cursor.y);
    return *this;
}

Render& Render::setPixel(Vector2i pos, Color color)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    _bitmap->set(pos, toBool(color));
    return *this;
}

Render& Render::drawVLine(int x, int y, int length)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    for (int32_t i = y; i < i + y; i++)
        setPixel({(int16_t)x, (int16_t)i}, Color::eBlack);

    return *this;
}

Render& Render::drawHLine(int x, int y, int length)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    for (int32_t i = x; i < i + x; i++)
        setPixel({(int16_t)i, (int16_t)y}, Color::eBlack);

    return *this;
}

Render& Render::drawLine(Rect2i rect)
{
    assert(_bitmap && "Bitmap must be set before drawing!");
    
    return *this;
}

Render& Render::drawRect(Rect2i rect)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    return *this;
}

Render& Render::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      setPixel({(int16_t)(x0 + x), (int16_t)(y0 + y)}, Color::eBlack);
      setPixel({(int16_t)(x0 + y), (int16_t)(y0 + x)}, Color::eBlack);
    }
    if (cornername & 0x2) {
      setPixel({(int16_t)(x0 + x), (int16_t)(y0 - y)}, Color::eBlack);
      setPixel({(int16_t)(x0 + y), (int16_t)(y0 - x)}, Color::eBlack);
    }
    if (cornername & 0x8) {
      setPixel({(int16_t)(x0 - y), (int16_t)(y0 + x)}, Color::eBlack);
      setPixel({(int16_t)(x0 - x), (int16_t)(y0 + y)}, Color::eBlack);
    }
    if (cornername & 0x1) {
      setPixel({(int16_t)(x0 - y), (int16_t)(y0 - x)}, Color::eBlack);
      setPixel({(int16_t)(x0 - x), (int16_t)(y0 - y)}, Color::eBlack);
    }
  }
}

Render& Render::drawCircle(uint16_t radius)
{
    assert(_bitmap && "Bitmap must be set before drawing!");



    return *this;
}

Render& Render::drawRoundedRect(int x, int y, int w, int h, int r)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius)
        r = max_radius;


    // smarter version
    drawHLine(x + r, y, w - 2 * r);         // Top
    drawHLine(x + r, y + h - 1, w - 2 * r); // Bottom
    drawVLine(x, y + r, h - 2 * r);         // Left
    drawVLine(x + w - 1, y + r, h - 2 * r); // Right
    
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1);
    drawCircleHelper(x + w - r - 1, y + r, r, 2);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
    drawCircleHelper(x + r, y + h - r - 1, r, 8);

    return *this;
}

Render& Render::drawText(const char* text)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    _ofr.printf("%s", text);
    return *this;
}

Render& Render::drawTextFormat(const char* format, ...)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    va_list va;
    va_start(va, format);
    _ofr.printf(format, va);
    va_end(va);

    return *this;
}

Render& Render::drawTextFromFlash(Strings::Select selection)
{
    assert(_bitmap && "Bitmap must be set before drawing!");

    char buffer[128+1];
    memset(buffer, 0, sizeof(buffer));
    Strings::copyTo(selection, buffer, sizeof(buffer)-1);
    _ofr.printf("%s", buffer);
    return *this;
}

bool Render::loadFont(const unsigned char *data, size_t size)
{
    _ofr.unloadFont();
    return !_ofr.loadFont(data, size);
}

bool Render::loadFont(const char* path)
{
    _ofr.unloadFont();
    return !_ofr.loadFont(path);
}

void Render::unloadFont()
{
    _ofr.unloadFont();
}

uint16_t Render::calculateFitFontSize(uint16_t maxWidth, uint16_t maxHeight, const char *str)
{
    return (uint16_t)_ofr.calculateFitFontSize(maxWidth, maxHeight, Layout::Horizontal, str);
}

Rect2i Render::calculateTextBox(Vector2i position, uint16_t fontSize, RenderAlign align, const char* str)
{
    FT_BBox bbox = _ofr.calculateBoundingBox(position.x, position.y, fontSize, ofr_alignConvert(align), Layout::Horizontal, str);

    /* Converts from FreeType's FT_BBox to our Rect2i. */
    return Rect2i{
        Vector2i{bbox.xMin, bbox.yMin}, 
        Extent2i{bbox.xMax - bbox.xMin, bbox.yMax - bbox.yMin}};
}

Align Render::ofr_alignConvert(RenderAlign align)
{
    switch (align) {
    case RenderAlign::eTopLeft: return Align::TopLeft;
    case RenderAlign::eTopCenter: return Align::TopCenter;
    case RenderAlign::eTopRight: return Align::TopRight;
    case RenderAlign::eMiddleLeft: return Align::MiddleLeft;
    case RenderAlign::eMiddleCenter: return Align::MiddleCenter;
    case RenderAlign::eMiddleRight: return Align::MiddleRight;
    case RenderAlign::eBottomLeft: return Align::BottomLeft;
    case RenderAlign::eBottomCenter: return Align::BottomCenter;
    case RenderAlign::eBottomRight: return Align::BottomRight;
    default: return Align::TopLeft;
    }
}

void Render::ofr_print(const char* str)
{
    printf("%s", str);
}

void Render::ofr_drawPixel(int32_t x, int32_t y, uint16_t c)
{
    _bitmap->set(ts::Vector2i{(int16_t)x, (int16_t)y}, c);
}

} /* namespace ts */