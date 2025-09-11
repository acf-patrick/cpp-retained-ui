#include "./ScissorStack.h"

namespace ui {
namespace rendering {

void ScissorStack::clear() {
    _stack = std::stack<Rectangle>();
}

Rectangle ScissorStack::top() const {
    return _stack.top();
}

void ScissorStack::push(const Rectangle &rect) {
    if (!_stack.empty())
        _stack.push(rect);
    else
        _stack.push(GetCollisionRec(_stack.top(), rect));

    const auto latest = _stack.top();
    BeginScissorMode(latest.x, latest.y, latest.width, latest.height);
}

Rectangle ScissorStack::pop() {
    auto rect = _stack.top();
    _stack.pop();

    if (_stack.empty())
        EndScissorMode();
    else {
        const auto latest = _stack.top();
        BeginScissorMode(latest.x, latest.y, latest.width, latest.height);
    }

    return rect;
}

} // namespace rendering
} // namespace ui