#pragma once

#include "../../utils/types.h"
#include <optional>

namespace ui {
namespace style {

struct Position {
  std::optional<utils::ValueRatioAuto<float>> left;
  std::optional<utils::ValueRatioAuto<float>> top;
  std::optional<utils::ValueRatioAuto<float>> right;
  std::optional<utils::ValueRatioAuto<float>> bottom;
};

}  // namespace style
}  // namespace ui