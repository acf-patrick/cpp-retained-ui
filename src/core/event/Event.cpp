#include "./Event.h"

const std::string event::Event::subtypeNames[] = {
    "MouseDownEvent",
    "MouseUpEvent",
    "ClickEvent",
    "DoubleClickEvent",
    "MouseMoveEvent",
    "MouseLeaveEvent",
    "MouseEnterEvent",
    "MouseOutEvent",
    "MouseOverEvent",
    "MouseWheenEvent",
    "KeyDownEvent",
    "KeyUpEvent"};

namespace event {

std::string Event::getName() const {
    return subtypeNames[_data.index()];
}

} // namespace event