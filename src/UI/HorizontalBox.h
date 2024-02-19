#pragma once

#include "Node.h"

namespace ts {

template<int N>
class HorizontalBox : public Container<N> {
public:
    constexpr HorizontalBox(std::array<Node*, N> children)
        : Container<N>(children) { }

    constexpr virtual void resize(Rect2i rect) override {
        Container<N>::resize(rect);

        /* the size of each child is (extent.x/n) */
        const Extent2i childWidth = { rect.extent.x/N, 0 };
        const Extent2i extent = { childWidth.x, rect.extent.y };
        
        Vector2i offset = rect.offset;
        Rect2i childRect = Rect2i{offset, extent};

        for (Node* child : this->_children) {
            child->resize(childRect);

            offset = rect.offset + childWidth;
            childRect = Rect2i{ offset, extent };
        }
    }
};

} /* namespace ts */