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

void Event::stopImmediatePropagation() {
    _stopImmediatePropagationFlag = true;
}

void Event::stopPropagation() {
    _stopPropagationFlag = true;
}

void Event::setTarget(std::shared_ptr<ui::element::Element> target) {
    _target = target;
}

void Event::setCurrentTarget(std::shared_ptr<ui::element::Element> target) {
    _currentTarget = target;
}

std::string Event::getName() const {
    return subtypeNames[_data.index()];
}

} // namespace event