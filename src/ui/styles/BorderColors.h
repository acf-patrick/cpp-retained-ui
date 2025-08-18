#pragma once

#include <raylib.h>

#include <optional>

namespace ui {
namespace style {

struct BorderColors {
  std::optional<Color> top;
  std::optional<Color> bottom;
  std::optional<Color> left;
  std::optional<Color> right;

  bool operator==(const BorderColors&) const = default;
};

}  // namespace style
}  // namespace ui