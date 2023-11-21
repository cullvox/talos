#pragma once

#include "Vector.h"

namespace ts {

struct Rect2i {
    Vector2i offset;
    Extent2i extent;

    inline bool isOverlapping(const Rect2i& other) const 
    {
        //isOverlapping = (x1min < x2max && x2min < x1max && y1min < y2max && y2min < y1max)

        return (offset.x < other.extent.width && other.offset.x < extent.width && offset.y < other.extent.height && other.offset.y < offset.y); 
    }

};

} // namespace talos