#pragma once

#include <optional>
#include <variant>

namespace ui {
namespace style {

enum class FlexDirection { Row, Column, RowReverse, ColumnReverse };

enum class JustifyContent {
  FlexStart,
  Center,
  FlexEnd,
  SpaceBetween,
  SpaceAround,
  SpaceEvenly
};

enum class Alignment { FlexStart, Center, FlexEnd, Stretch, Baseline, Auto };

struct FlexBasisPercent {
  float value;

  bool operator<=>(const FlexBasisPercent&) const = default;
};

struct FlexBasisValue {
  float value;

  bool operator<=>(const FlexBasisValue&) const = default;
};

struct FlexBasisAuto { bool operator<=>(const FlexBasisAuto&) const = default; };

struct Flex {
  std::optional<FlexDirection> flexDirection;
  std::optional<JustifyContent> justifyContent;
  std::optional<Alignment> alignItems;
  std::optional<Alignment> alignSelf;
  std::optional<float> flexGrow;
  std::optional<float> flexShrink;
  std::optional<std::variant<FlexBasisAuto, FlexBasisPercent, FlexBasisValue>>
      flexBasis;
  std::optional<float> gap; // for both horizontal and vertical
  std::optional<float> rowGap;
  std::optional<float> columnGap;
  std::optional<float> gapRatio; // ratio of parent's size for both horizontal and vertial gaps
  std::optional<float> rowGapRatio;
  std::optional<float> columnGapRatio;

  bool operator<=>(const Flex&) const = default;
};

}  // namespace style
}  // namespace ui