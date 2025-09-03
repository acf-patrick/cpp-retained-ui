#pragma once

#include "../utils/types.h"

#include <optional>
#include <raylib.h>

namespace ui {
namespace style {

struct Transform {
    struct Translate {
        utils::ValueRatio<float> x;
        utils::ValueRatio<float> y;
    };

    struct Rotate {
        utils::Angle angle;
    };

    std::optional<Translate> translation;
    std::optional<Rotate> rotation;
    std::optional<Vector2> scale;
};

} // namespace style
} // namespace ui