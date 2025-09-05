#pragma once

#include "../utils/types.h"
#include "../utils/operators.h"

#include <optional>
#include <raylib.h>

namespace ui {
namespace style {

struct Transform {
    struct Translate {
        utils::ValueRatio<float> x;
        utils::ValueRatio<float> y;

        bool operator<=>(const Translate&) const = default;
    };

    struct Rotate {
        utils::Angle angle;

        bool operator<=>(const Rotate&) const = default;
    };

    std::optional<Translate> translation;
    std::optional<Rotate> rotation;
    std::optional<Vector2> scale;

    bool operator<=>(const Transform&) const = default;
};

} // namespace style
} // namespace ui