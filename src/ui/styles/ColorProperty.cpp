#include "./ColorProperty.h"

bool operator==(const ui::style::ColorProperty& lhs,
                const ui::style::ColorProperty& rhs) {
  if (ui::style::helper::isColorInherited(lhs) &&
      ui::style::helper::isColorInherited(rhs))
    return true;

  if (auto colorA = std::get_if<Color>(&lhs)) {
    if (auto colorB = std::get_if<Color>(&rhs)) {
      return colorA->r == colorB->r && colorA->g == colorB->g &&
             colorA->b == colorB->b && colorA->a == colorB->a;
    }
  }

  return false;
}

bool operator!=(const ui::style::ColorProperty& lhs, const ui::style::ColorProperty& rhs) {
  return !(lhs == rhs);
}

namespace ui {
namespace style {
namespace helper {

bool isColorInherited(const ColorProperty& color) {
  return std::holds_alternative<Inherit>(color);
}

}  // namespace helper
}  // namespace style
}  // namespace ui