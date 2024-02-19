#pragma once

#include "Container.h"

namespace ts {

template<int N>
class VerticalBox : public Container<N> {
public:
    constexpr VerticalBox(std::array<Node*, N> children)
        : Container<N>(children) { }

    constexpr virtual void resize(Rect2i rect) override {
        Container<N>::resize(rect);

        /* the size of each child is (extent.y/n) */
        const Extent2i childHeight = { 0, rect.extent.y/N };
        const Extent2i extent = { rect.extent.x, childHeight.y };
        
        Vector2i offset = rect.offset;
        Rect2i childRect = Rect2i{offset, extent};

        for (Node* child : this->_children) {
            child->resize(childRect);

            offset = rect.offset + childHeight;
            childRect = Rect2i{offset, extent};
        }
    }
};

} /* namespace ts */