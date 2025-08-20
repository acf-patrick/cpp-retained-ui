#pragma once

#include "../../utils/types.h"
#include "./BorderColors.h"
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
    std::optional<utils::ValueRatio<float>> borderRadius; // work only with Layout.spacing.border set (even border thickness)

    bool operator<=>(const Style &) const = default;
};

} // namespace style
} // namespace ui