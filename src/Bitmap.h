#pragma once

#include "Result.h"
#include "Vector.h"
#include "Rect.h"
#include "Prone.h"

namespace ts {

class BitmapInterface {
public:

    /** @brief Returns the extent of the bitmap. */
    virtual Extent2i extent() const = 0;

    /** @brief Returns the data of the bitmap. */
    virtual const uint8_t* data() const = 0;
    virtual uint8_t* data() = 0;
    
    /** @brief Sets one bit on or off. */
    void set(Vector2i position, bool value);

    /** @brief Gets a bit at a value. */
    bool get(Vector2i position) const;
    
    /** @brief Binary blit two images together. */
    void blit(const BitmapInterface& src, Vector2i offset, bool override = false);

    /** @brief Clears the value of the bitmap. */
    void clear(uint8_t value = 0xFF);

    /** @brief Flips every bit in this bitmap. */
    void flip();
 
    inline uint32_t sizeBytes() 
    {
        Extent2i ext = extent();
        uint32_t _widthBytes = ((ext.width % 8 == 0) ? (ext.width / 8) : (ext.width / 8 + 1));
        uint32_t _heightBytes = ext.height;
        return _widthBytes * _heightBytes;
    } 

};

/** @brief Safe bitmap, cannot fail at runtime. */
template<int width, int height>
class Bitmap : public BitmapInterface {
public:

    /** @brief Default constructor. */
    Bitmap() = default;

    /** @brief Move constructor. */
    Bitmap(Bitmap&& rhs)
    {
        memcpy(_data, rhs._data, _sizeBytes);
        rhs.clear();
    }

    /** @brief Move-Any-Bitmap constructor. */
    Bitmap(const BitmapInterface&& rhs)
    {
        assert(rhs.extent() == _extent && "Extents must be the same!"); 
        assert(rhs.data() != nullptr && "Must be a valid and created bitmap!");

        memcpy(_data, rhs.data(), _sizeBytes);
    }

    /** @brief Data constructor. */
    Bitmap(const uint8_t* buffer)
    {
        memcpy(_data, buffer, _sizeBytes);
    }

    virtual Extent2i extent() const override { return Extent2i{width, height}; }
    virtual const uint8_t* data() const override { return _data; }
    virtual uint8_t* data() override { return _data; }
    
private:
    static inline constexpr Extent2i _extent = { width, height };
    static inline constexpr uint32_t _widthBytes = ((width % 8 == 0) ? (width / 8) : (width / 8 + 1));
    static inline constexpr uint32_t _heightBytes = height;
    static inline constexpr uint32_t _sizeBytes = _widthBytes * _heightBytes;
    uint8_t _data[_sizeBytes];
};

/** @brief Normal allocating bitmap, fails on allocation failure. */
class BitmapAlloc : public BitmapInterface, public Prone {
public:

    /** @brief Default constructor. */
    BitmapAlloc() = default;

    /** @brief Move constructor. */
    BitmapAlloc(BitmapAlloc&& rhs);

    /** @brief Creates a cleared bitmap.
     *  @param extent Extent in pixels.
     */
    BitmapAlloc(Extent2i extent, bool usePsRam = false);

    /** @brief Creates a bitmap with values from memory. 
     *  @param extent Extent in pixels, two byte alignment enforced. 
     *  @param buffer Bit buffer to read from, must be sizeof (extent.width*extent.height)/8.
    */
    BitmapAlloc(Extent2i extent, const uint8_t* buffer, bool usePsRam = false);

    /** @brief Destroys the this bitmap's memory. */
    ~BitmapAlloc();

    /** @brief Moves the bitmap. */
    BitmapAlloc& operator=(BitmapAlloc&& rhs);

    virtual Extent2i extent() const override { return _extent; }
    virtual uint8_t* data() override { return _data; }
    virtual const uint8_t* data() const override { return _data; }

    /** @brief Construction result value. */
    Result what() const { return _what; };

private:
    Extent2i _extent;
    uint32_t _widthBytes;
    uint32_t _heightBytes;
    uint32_t _sizeBytes;
    uint8_t* _data;
};

} // namespace ts