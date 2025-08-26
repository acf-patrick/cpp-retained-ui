#pragma once

#include "./Event.h"

#include <elements/Element.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace event {

class EventManager {
    struct Cache {
        Vector2 mousePosition;
        std::shared_ptr<ui::element::Element> lastHovered;          // in context of `MouseLeave`, `MouseEnter`, `MouseOut` and `MouseOver`
        std::unordered_map<MouseButton, std::uint64_t> clickedTime; // last click time in milliseconds
    };

    std::weak_ptr<ui::element::Element> _root;
    std::vector<std::shared_ptr<Event>> _events;
    Cache _cache;

  public:
    EventManager(std::shared_ptr<ui::element::Element> root);

    // Update cached events
    // @param dt delta-time in milliseconds
    void update(std::uint64_t dt);
};

} // namespace event