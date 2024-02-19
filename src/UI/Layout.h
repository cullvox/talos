#pragma once

#include "Node.h"

namespace ts {
class Layout : public Node {
    Extent2i _extent;
    Node* _child;

public:
    constexpr Layout(Extent2i extent, Node* child)
        : _extent(extent)
        , _child(child) { }

    constexpr virtual void resize(Rect2i rect) override {
        Node::resize(rect);
        
        /* the child of the layout is always the same size as the layout. */
        _child->resize(rect);
    }
           
    virtual void render(Render& render) {
        _child->render(render);
    }

};

} /* namespace ts */