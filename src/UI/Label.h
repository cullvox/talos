#pragma once

#include <string>
#include <Arduino.h>

#include "Node.h"

namespace ts {

class Label : public Node {
private:
    std::string _text;
    u16 _fontSize;

public:
    constexpr Label() = default;
    constexpr ~Label() = default;

    constexpr virtual void resize(Rect2i rect) override {
        Node::resize(rect);
    }
    constexpr void setText(const std::string& text) { _text = text; }
    constexpr void setFontSize(u16 fontSize) { _fontSize = fontSize; }     
    
    virtual void render(Render& render) override {

        render
            .setAlignment(RenderAlign::eTopLeft)
            .setCursor(_rect.offset)
            .setFillColor(Color::eBlack)
            .setOutlineColor(Color::eWhite)
            .drawText(_text.c_str());

    }


};

} /* namespace ts */