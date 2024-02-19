#pragma once

#include "Numeric.h"

namespace ts {

template<typename T = int>
struct Vector2 {
public:
    T x;
    T y;

    constexpr Vector2()         { x = y = {}; };
    constexpr Vector2(T xy)     { x = y = xy; }
    constexpr Vector2(T x, T y) { x = x; y = y; }

    static constexpr Vector2 make()         { return Vector2{}; }
    static constexpr Vector2 make(T xy)     { return Vector2{xy}; }
    static constexpr Vector2 make(T x, T y) { return Vector2{x, y}; }  

    constexpr Vector2 zero()    { return Vector2{(T)0}; }
    constexpr Vector2 one()     { return Vector2{(T)1};  }

public:

    /* Assignment Operators */
    constexpr Vector2 operator=(const Vector2& rhs) { x = rhs.x; y = rhs.y; return *this; }

    /* Math/Assignment Operators */
    constexpr Vector2 operator+(const Vector2& rhs) const { return Vector2{x + rhs.x, y + rhs.y}; }
    constexpr Vector2 operator-(const Vector2& rhs) const { return Vector2{x - rhs.x, y - rhs.y}; }
    constexpr Vector2 operator*(const Vector2& rhs) const { return Vector2{x * rhs.x, y * rhs.y}; }
    constexpr Vector2 operator/(const Vector2& rhs) const { return Vector2{x / rhs.x, y / rhs.y}; }

    constexpr Vector2 operator+(T rhs) const { return Vector2{x + rhs, y + rhs}; }
    constexpr Vector2 operator-(T rhs) const { return Vector2{x - rhs, y - rhs}; }
    constexpr Vector2 operator*(T rhs) const { return Vector2{x * rhs, y * rhs}; }
    constexpr Vector2 operator/(T rhs) const { return Vector2{x / rhs, y / rhs}; }

    constexpr Vector2& operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vector2& operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vector2& operator*=(const Vector2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
    constexpr Vector2& operator/=(const Vector2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }

    constexpr Vector2& operator+=(T rhs) { x += rhs; y += rhs; return *this; }
    constexpr Vector2& operator-=(T rhs) { x -= rhs; y -= rhs; return *this; }
    constexpr Vector2& operator*=(T rhs) { x *= rhs; y *= rhs; return *this; }
    constexpr Vector2& operator/=(T rhs) { x /= rhs; y /= rhs; return *this; }

    /* Comparison Operators */
    constexpr bool operator==(const Vector2& rhs) { return (x == rhs.x && y == rhs.y); }
    constexpr bool operator!=(const Vector2& rhs) { return (x != rhs.x || y != rhs.y); }

};

template<typename T>
using Extent2 = Vector2<T>;

using Extent2i = Extent2<s32>;
using Extent2f = Extent2<f32>;

using Vector2i = Vector2<s32>;
using Vector2f = Vector2<f32>;

} // namespace ts
