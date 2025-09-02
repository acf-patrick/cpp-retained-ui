#pragma once

#include "../../utils/types.h"
#include "./BorderColors.h"
#include "./DrawableContentProps.h"
#include "./Inheritables.h"
#include "./Transform.h"
#include "./TransformOrigin.h"
#include "./Isolation.h"

#include <optional>
#include <raylib.h>

namespace ui {
namespace style {

struct Style {
    Inheritables inheritables;
    float opacity;
    int zIndex;
    Isolation isolation; // isolate : Element forces a new stacking context to isolate blending
    std::optional<Color> backgroundColor;
    std::optional<Color> borderColor;
    std::optional<BorderColors> borderColors;
    std::optional<Transform> transform;
    TransformOrigin transformOrigin; // default to center
    std::optional<utils::ValueRatio<float>> borderRadius;     // work only with Layout.spacing.border set (even border thickness)
    std::optional<DrawableContentProps> drawableContentProps; // properties for drawable content like image element

    bool operator<=>(const Style &) const = default;
};

} // namespace style
} // namespace ui