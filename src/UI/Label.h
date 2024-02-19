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

    constexpr virtual void resize(Rect2i rect) {
        Node::resize(rect);
    }
    
    void setText(const String& text) { _text = text; }
    void setFontSize()


};

} /* namespace ts */