#pragma once

#include "../../utils/types.h"
#include "./ColorProperty.h"
#include "./BorderColors.h"

#include <raylib.h>
#include <optional>

namespace ui {
namespace style {

struct Style {
  ColorProperty color;
  std::optional<Color> backgroundColor;
  std::optional<Color> borderColor;
  std::optional<BorderColors> borderColors;
  std::optional<utils::ValueRatio<float>> borderRadius; // work only with Layout.spacing.border set (even border thickness)

  bool operator<=>(const Style&) const = default;
};

}  // namespace style
}  // namespace ui