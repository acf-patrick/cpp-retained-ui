#pragma once

#include <raylib.h>
#include <variant>

namespace ui {
namespace style {

struct Inherit {
  bool operator<=>(const Inherit&) const = default;
};

using ColorProperty = std::variant<Color, Inherit>;

namespace helper {

bool isColorInherited(const ColorProperty& color);

}  // namespace helper

}  // namespace style
}  // namespace ui

bool operator==(const ui::style::ColorProperty& lhs,
                const ui::style::ColorProperty& rhs);

bool operator!=(const ui::style::ColorProperty& lhs,
                const ui::style::ColorProperty& rhs);