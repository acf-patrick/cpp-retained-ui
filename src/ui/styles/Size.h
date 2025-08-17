#pragma once

#include "../../utils/types.h"
#include <optional>

namespace ui {
namespace style {

  struct Size {
    std::optional<utils::ValueRatioAuto<int>> width;   // set explicit width
    std::optional<utils::ValueRatioAuto<int>> height;  // set explicit height
    std::optional<int> minWidth;
    std::optional<int> minHeight;
    std::optional<int> maxWidth;
    std::optional<int> maxHeight;
    std::optional<float>
        aspectRatio;  // if only width or height is set, the other dimension is
                      // computed from aspectRatio.

    bool operator<=>(const Size&) const = default;
  };
  
}  // namespace style
}