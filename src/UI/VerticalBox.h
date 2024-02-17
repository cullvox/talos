#pragma once

#include "Node.h"


namespace ts {
class VerticalBox : public Node {
public:
    constexpr VerticalBox(Node* children, u32 num)
        : Node(children) {
        
        for (Node* child : children) {
            child->_
        }
    }
};

} /* namespace ts */