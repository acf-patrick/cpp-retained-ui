#pragma once

#include "../../utils/types.h"

#include <optional>

namespace ui {
namespace style {

struct Spacing {
  std::optional<utils::ValueRatioAuto<int>>
      margin;  // margin for all four directions
  std::optional<utils::ValueRatioAuto<int>> marginLeft;
  std::optional<utils::ValueRatioAuto<int>> marginHorizontal;
  std::optional<utils::ValueRatioAuto<int>> marginVertical;
  std::optional<utils::ValueRatioAuto<int>> marginRight;
  std::optional<utils::ValueRatioAuto<int>> marginTop;
  std::optional<utils::ValueRatioAuto<int>> marginBottom;

  std::optional<utils::ValueRatio<int>> padding;  // padding for all four directions
  std::optional<utils::ValueRatio<int>> paddingHorizontal; 
  std::optional<utils::ValueRatio<int>> paddingVertical; 
  std::optional<utils::ValueRatio<int>> paddingLeft;
  std::optional<utils::ValueRatio<int>> paddingRight;
  std::optional<utils::ValueRatio<int>> paddingTop;
  std::optional<utils::ValueRatio<int>> paddingBottom;

  std::optional<float> border;  // border for all four directions
  std::optional<float> borderLeft;
  std::optional<float> borderRight;
  std::optional<float> borderTop;
  std::optional<float> borderBottom;

  bool operator<=>(const Spacing&) const = default;
};

}  // namespace style
}  // namespace ui