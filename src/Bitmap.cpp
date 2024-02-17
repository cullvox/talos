#include <memory.h>
#include <utility>

#include <Arduino.h>

#include "Print.h"
#include "Bitmap.h"

namespace ts {

/* === BITMAP INTERFACE === */

bool BitmapInterface::get(Vector2i position) const
{
    int byteIndex = position.y * ((extent().x + 7) / 8); // Calculate the starting byte of the row
    int bitOffset = position.x % 8; // Calculate the bit offset within the byte
    int byteOffset = position.x / 8; // Calculate the byte offset within the row

    // Shift the byte to the right by the bit offset and extract the bit
    return (data()[byteIndex + byteOffset] & (1 << (7 - bitOffset))) != 0;
}

void BitmapInterface::set(Vector2i position, bool value)
{
    const Extent2i ext = extent();
    if (position.x >= ext.x ||
        position.y >= ext.y) return;

    int byteIndex = position.y * ((ext.x + 7) / 8); // Calculate the starting byte of the row
    int bitOffset = position.x % 8; // Calculate the bit offset within the byte
    int byteOffset = position.x / 8; // Calculate the byte offset within the row

    // Shift 0x01 to the left by the bit offset and create a mask
    unsigned char mask = 1 << (7 - bitOffset);
    if (value) 
    {
        // Set the bit to 1
        data()[byteIndex + byteOffset] |= mask;
    } else 
    {
        // Clear the bit to 0
        data()[byteIndex + byteOffset] &= ~mask;
    }
}

void BitmapInterface::blit(const BitmapInterface& src, Vector2i offset, bool override)
{
    int startX = offset.x;
    int startY = offset.y;

    const Extent2i ext = extent();
    Extent2i srcExtent = src.extent();
    int srcWidth = srcExtent.x;
    int srcHeight = srcExtent.y;

    for (uint16_t x = 0; x < srcWidth && x < ext.x; x++)
    {
        for (uint16_t y = 0; y < srcHeight && y < ext.x; y++)
        {
            Extent2i srcPosition{x, y};
            Extent2i destPosition{
                (uint16_t)(startX + x),
                (uint16_t)(startY + y)};

            bool value = src.get(srcPosition);
            if (!value || override)
                set(destPosition, value);
        }
    }
}

void BitmapInterface::clear(uint8_t value) 
{
    memset(data(), value, sizeBytes());
}

void BitmapInterface::flip()
{
    for (uint32_t i = 0; i < sizeBytes(); i++)
    {
        data()[i] = ~data()[i];
    }
}

BitmapAlloc::BitmapAlloc(BitmapAlloc&& rhs)
    : _extent(std::move(rhs._extent))
    , _widthBytes(std::move(rhs._widthBytes))
    , _heightBytes(std::move(rhs._heightBytes))
    , _sizeBytes(std::move(rhs._sizeBytes))
    , _data(std::move(rhs._data))
{
}

BitmapAlloc::BitmapAlloc(Extent2i extent, bool usePsRam)
    : _extent(extent)
    , _widthBytes(((extent.x % 8 == 0) ? (extent.x / 8 ) : (extent.x / 8 + 1)))
    , _heightBytes(extent.y)
    , _sizeBytes(_widthBytes * _heightBytes)
    , _data(nullptr)
{
    assert(extent.x <= 1024 && "Bitmap width must be less than 1024!");
    assert(extent.y <= 1024 && "Bitmap height must be less than 1024!");

    log_d("Allocating bitmap of width %d, height %d.", extent.x, extent.y);
    
    if (usePsRam && !psramFound())
    {
        assert(false && "Cannot use PSRAM on system where it is not available!");
        return;
    }

    if (usePsRam)
        _data = (uint8_t*)ps_calloc(1, _sizeBytes);
    else 
        _data = (uint8_t*)calloc(1, _sizeBytes);

    if (!_data) 
    {
        log_e("Could not allocate memory for bitmap!");
        return;
    }
}

BitmapAlloc::BitmapAlloc(Extent2i extent, const uint8_t* buffer, bool usePsRam) 
    : BitmapAlloc(extent, usePsRam)
{  
    assert(buffer && "Bitmap buffer must be a valid pointer!");
    if (bad()) return;

    memcpy(_data, buffer, _sizeBytes);
}

BitmapAlloc::~BitmapAlloc()
{
    free(_data);
}

BitmapAlloc& BitmapAlloc::operator=(BitmapAlloc&& rhs)
{
    _extent = rhs._extent;
    _widthBytes = rhs._widthBytes;
    _heightBytes = rhs._heightBytes;
    _sizeBytes = rhs._sizeBytes;
    _data = rhs._data;

    rhs._extent = {};
    rhs._widthBytes = {};
    rhs._heightBytes = {};
    rhs._sizeBytes = {};
    rhs._data = {};

    return *this;
}

} /* namespace talos */