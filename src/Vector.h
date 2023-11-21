#pragma once

#include <stdint.h>

namespace ts {

struct Extent2i;

struct Vector2i {
    int16_t x;
    int16_t y;

    Vector2i() = default;
    ~Vector2i() = default;

    static inline Vector2i zero() { return Vector2i{0, 0}; }
    
    inline Vector2i operator+(const Vector2i& rhs) const { return Vector2i{ (int16_t)(x + rhs.x), (int16_t)(y + rhs.y)}; }
    inline Vector2i operator+(int16_t value) const { return Vector2i{ (int16_t)(x + value), (int16_t)(y + value)}; }
    inline Vector2i operator-(const Vector2i& rhs) const { return Vector2i{ (int16_t)(x - rhs.x), (int16_t)(y - rhs.y)}; }
    inline Vector2i operator-(int16_t value) const { return Vector2i{ (int16_t)(x - value), (int16_t)(y - value)}; }
    inline Vector2i operator*(const Vector2i& rhs) const { return Vector2i{ (int16_t)(x * rhs.x), (int16_t)(y * rhs.y)}; }
    inline Vector2i operator*(int16_t scalor) const { return Vector2i{ (int16_t)(x * scalor), (int16_t)(y * scalor)}; }
    inline Vector2i operator/(const Vector2i& rhs) const { return Vector2i{ (int16_t)(x / rhs.x), (int16_t)(y / rhs.y)}; }
    inline Vector2i operator/(int16_t value) const { return Vector2i{ (int16_t)(x / value), (int16_t)(y / value)}; }
    inline operator Extent2i() const;
};

struct Extent2i {
    uint16_t width;
    uint16_t height;

    Extent2i() = default;
    ~Extent2i() = default;

    static inline Extent2i zero() { return Extent2i{0, 0}; }

    inline bool operator==(const Extent2i& rhs) const { return (width == rhs.width) && (height == rhs.height); }
    inline operator Vector2i() const { return Vector2i{ (int16_t)width, (int16_t)height}; }
    inline Extent2i operator+(const Extent2i& rhs) const { return Extent2i{ (uint16_t)(width + rhs.width), (uint16_t)(height + rhs.height)}; }
    inline Extent2i operator-(const Extent2i& rhs) const { return Extent2i{ (uint16_t)(width - rhs.width), (uint16_t)(height - rhs.height)}; }
};

Vector2i::operator ts::Extent2i() const { return Extent2i{(uint16_t)x, (uint16_t)y}; }

} // namespace ts
