#pragma once

#include "../utils/types.h"

namespace ui {
namespace style {

struct TransformTranslate {
    utils::ValueRatio<float> x;
    utils::ValueRatio<float> y;
};

struct TransformRotate {
    utils::Angle angle;
};

struct TransformScale {
    utils::ValueRatio<float> x;
    utils::ValueRatio<float> y;
};

using Transform = std::variant<TransformTranslate, TransformRotate, TransformScale>;

} // namespace style
} // namespace ui