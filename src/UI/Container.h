#pragma once

namespace ts {

template<int N>
class Container {
protected:
    std::array<Node*, N> _children;
public:
    constexpr Container(std::array<Node*, N> children)
        : _children(children) {}
    
    virtual void render(Render& render) override {
        for (Node* child : _children) {
            child->render(render);
        }
    }
};


} /* namespace ts */