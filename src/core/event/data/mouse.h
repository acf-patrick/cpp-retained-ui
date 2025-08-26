#pragma once

#include <elements/Element.h>

#include <raylib.h>
#include <memory>
#include <vector>

namespace event {
namespace data {

struct MouseDown {
    Vector2 position; // absolute position
    MouseButton button;
};

struct MouseUp {
    Vector2 position; // absolute position
    MouseButton button;
};

// Complete press then release occured
struct Click {
    Vector2 position; // absolute position
    MouseButton button;
};

struct DoubleClick {
    Vector2 position; // absolute position
    MouseButton button;
};

struct MouseMove {
    Vector2 position; // aboslute position
    Vector2 movement;
};

/*
| Event        | Fires on element  | Fires on children?   | Bubbles?  |
| ------------ | ----------------- | -------------------  | --------- |
| `mouseover`  | ✅ yes            | ✅ yes              | ✅ yes    |
| `mouseenter` | ✅ yes            | ❌ no               | ❌ no     |
| `mouseout`   | ✅ yes            | ✅ yes              | ✅ yes    |
| `mouseleave` | ✅ yes            | ❌ no               | ❌ no     |
*/

struct MouseLeave {
    Vector2 position;
    std::shared_ptr<ui::element::Element> relatedTarget; // element mouse goes to
};

struct MouseEnter {
    Vector2 position;
    std::shared_ptr<ui::element::Element> relatedTarget; // element mouse came from
};

struct MouseOut {
    Vector2 position;
    std::shared_ptr<ui::element::Element> relatedTarget; // element mouse goes to
};

struct MouseOver {
    Vector2 position;
    std::shared_ptr<ui::element::Element> relatedTarget; // element mouse came from
};

//////////////////////////////////////////////////////////////////////////

struct MouseWheel {
    Vector2 position;
    Vector2 delta; // amount of scroll
};

// TODO : drag/drop event datas

} // namespace data
} // namespace event