#pragma once

#include <list>
#include <memory>
#include <unordered_map>

class SlideController {
public:
    
    Result next(Render& render);

    void add(std::unique_ptr<Slide> slide);
    void remove(std::string_view name);
    void disable(std::string_view name);
    void enable(std::string_view name);

private:
    struct SlideNode {
        bool enabled;
        std::unique_ptr<Slide> slide;
    };

    std::unordered_map<std::string, SlideNode> _slides;
};