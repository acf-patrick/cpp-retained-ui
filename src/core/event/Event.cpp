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
    "MouseWheelEvent",
    "KeyDownEvent",
    "KeyUpEvent"};

namespace event {

std::shared_ptr<ui::element::Element> Event::getTarget() const {
    return _target.lock();
}

std::shared_ptr<ui::element::Element> Event::getCurrentTarget() const {
    return _currentTarget.lock();
}

std::string Event::getName() const {
    return subtypeNames[_data.index()];
}

} // namespace event