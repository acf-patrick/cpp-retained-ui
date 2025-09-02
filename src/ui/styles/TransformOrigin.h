#pragma once

#include "../utils/types.h"

#include <variant>

namespace ui {
namespace style {

// use center of the element as transform-origin
struct TransformOriginCenter {};

struct TransformOriginPosition {
    utils::ValueRatio<int> x;
    utils::ValueRatio<int> y;
};

using TransformOrigin = std::variant<TransformOriginCenter, TransformOriginPosition>;

} // namespace style
} // namespace ui