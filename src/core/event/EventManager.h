#pragma once

#include <elements/Element.h>

#include <memory>

namespace event {

class EventManager {
    std::weak_ptr<ui::element::Element> _root;

public:
    EventManager(std::shared_ptr<ui::element::Element> root);

    void update(float dt);
};

}