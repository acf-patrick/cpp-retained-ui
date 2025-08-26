#include "./EventManager.h"

namespace event {

EventManager::EventManager(std::shared_ptr<ui::element::Element> root) : _root(root) {
}

void EventManager::update(std::uint64_t dt) {
    const auto mousePosition = GetMousePosition();
    const std::vector<MouseButton> mouseButtons = {
        MOUSE_BUTTON_BACK,
        MOUSE_BUTTON_EXTRA,
        MOUSE_BUTTON_FORWARD,
        MOUSE_BUTTON_LEFT,
        MOUSE_BUTTON_MIDDLE,
        MOUSE_BUTTON_RIGHT,
        MOUSE_BUTTON_SIDE};

    for (auto button : mouseButtons) {
        if (IsMouseButtonPressed(button))
            _events.emplace_back(Event::New(event::data::MouseDown{mousePosition, button}));

        if (IsMouseButtonReleased(button)) {
            _events.emplace_back(Event::New(event::data::MouseUp{mousePosition, button}));
        }
    }

    _cache.mousePosition = mousePosition;
}

} // namespace event