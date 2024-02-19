#pragma once

#include <array>

#include "Vector.h"
#include "Rect.h"
#include "Render.h"

namespace ts {

class Node {
protected:
    Rect2i _rect;
public:
    constexpr virtual void resize(Rect2i rect) { _rect = rect; }
    virtual void render(Render& render) { }
};

} /* namespace ts */