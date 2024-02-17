#pragma once

#include "Vector.h"

namespace ts {

template<typename T>
struct Rect2 {
    Vector2<T> offset;
    Extent2<T> extent;

    constexpr Rect2()
        : offset()
        , extent() {}

    constexpr Rect2(Vector2<T> offset, Extent2<T> extent)
        : offset(offset)
        , extent(extent) {}

    constexpr inline bool isWithin(Vector2<T> point) const {
        return (point.x > offset.x && point.x < extent.x) &&
               (point.y > offset.y && point.y < extent.y);
    }

    constexpr inline bool isOverlapping(const Rect2& other) const {
        return (offset.x < other.extent.x && 
                other.offset.x < extent.x && 
                offset.y < other.extent.y && 
                other.offset.y < offset.y); 
    }
};

#include "Numeric.h"

using Rect2i = Rect2<s32>;
using Rect2f = Rect2<f32>;

} // namespace talos