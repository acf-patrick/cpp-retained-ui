#pragma once

#include <raylib.h>
#include <vector>

namespace event {
namespace data {

enum class MouseButton {
    Left,
    Middle,
    Right
};

enum class MouseModifierKey {
    Ctrl,
    Shift,
    Alt,
    Meta
};

struct MouseDown {
    Vector2 position; // absolute position
    MouseButton button;
    std::vector<MouseButton> buttons; // pressed/released buttons
    std::vector<MouseModifierKey> modifiers;
};

struct MouseUp {
    Vector2 position; // absolute position
    MouseButton button;
    std::vector<MouseButton> buttons; // pressed/released buttons
    std::vector<MouseModifierKey> modifiers;
};

struct Click {
    Vector2 position; // absolute position
    MouseButton button;
    std::vector<MouseButton> buttons; // pressed/released buttons
};

struct DoubleClick {
    Vector2 position; // absolute position
    MouseButton button;
    std::vector<MouseButton> buttons; // pressed/released buttons
    std::vector<MouseModifierKey> modifiers;
};

} // namespace data
} // namespace event