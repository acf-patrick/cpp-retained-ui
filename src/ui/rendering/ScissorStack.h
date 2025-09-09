#pragma once

#include <stack>
#include <raylib.h>

namespace ui {
namespace rendering {

class ScissorStack {
    std::stack<Rectangle> _stack;

public:
    ScissorStack() = default;

    Rectangle top() const;
    void push(const Rectangle&);
    Rectangle pop();
};

}
}