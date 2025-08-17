#pragma once

#include <optional>
#include "../../utils/types.h"

namespace ui {
namespace style {

struct Position {
  std::optional<utils::ValueRatioAuto<float>> left;
  std::optional<utils::ValueRatioAuto<float>> top;
  std::optional<utils::ValueRatioAuto<float>> right;
  std::optional<utils::ValueRatioAuto<float>> bottom;

  bool operator<=>(const Position&) const = default;
};

}  // namespace style
}  // namespace ui