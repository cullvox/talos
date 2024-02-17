#pragma once

#include "Vector2.h"
#include "Rect2.h"

namespace ts {

class Node {
protected:
    Node* _children;
    u32 _numChildren;
    Rect2i _rect;

public:
    constepxr Node(Node* children, u32 num)
        : _children(children) {}

    constexpr virtual Node* getChildren() { return _children; }

    virtual void render(Render& render) {}              /* draws this and child nodes. */

};

} /* namespace ts */