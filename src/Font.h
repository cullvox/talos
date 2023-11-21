#pragma once

#include "Bitmap.h"
#include "Result.h"
#include "Prone.h"

namespace ts {

/** @brief A representation of a FreeType font object while using the stack for bitmap allocation.
 * 
 *  Schrift allocates fonts on PSRAM.
 * 
 */
template<int width>
class Font : public Prone {

    /* To prevent using too much memory, the maximum size is 100x100 pixels. 
        We are using stack for the glyph bitmap after all. */
    static_assert(width <= 100 && "Font width cannot be larger than 100 px");

public:
    Font();
    ~Font();

    Result loadFromFile(const char* pPath);
    Result loadFromMemory(uint32_t memorySize, const uint8_t* memory);
    Result render(uint32_t unicode);
    const BitmapInterface& getBitmap() const;

private:
    Bitmap<width, width>    _buffer;
};

} /* namespace ts */

#include "Font.inl"