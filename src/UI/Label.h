#pragma once

#include <Arduino.h>

#include "Node.h"

namespace ts {

class Label : public Node {
private:
    String _text;

public:

    Label(const char* text);
    ~Label();

    virtual void compute(Extent2f extent);
    
    void setText(const String& text) { _text = text; }
    

};

} /* namespace ts */