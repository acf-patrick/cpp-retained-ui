#pragma once

#include "../../utils/types.h"
#include "./BorderColors.h"
#include "./DrawableContentProps.h"
#include "./Inheritables.h"

#include <optional>
#include <raylib.h>

namespace ui {
namespace style {

struct Style {
    Inheritables inheritables;
    std::optional<Color> backgroundColor;
    std::optional<Color> borderColor;
    std::optional<BorderColors> borderColors;
    std::optional<utils::ValueRatio<float>> borderRadius;     // work only with Layout.spacing.border set (even border thickness)
    std::optional<DrawableContentProps> drawableContentProps; // properties for drawable content like image element

    bool operator<=>(const Style &) const = default;
};

} // namespace style
} // namespace ui