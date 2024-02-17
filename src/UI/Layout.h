#pragma once

#include "Node.h"

namespace ts {

class Layout : public Node {

    Extent2i _extent;

public:
    Layout(Extent2i extent);


    virtual void calculate();           
    virtual void render();

};

} /* namespace ts */