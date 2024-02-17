#pragma once

#include "Node.h"

namespace ts {

class HorizontalBox : public Node {
public:
    constexpr HorizontalBox(const Node* children);

    ~HorizontalBox();    

};

} /* namespace ts */