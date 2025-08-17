#pragma once

#include <optional>
#include "./Display.h"
#include "./Flex.h"
#include "./Overflow.h"
#include "./Position.h"
#include "./PositionType.h"
#include "./Size.h"
#include "./Spacing.h"

namespace ui {
namespace style {

struct Layout {
  std::optional<Size> size;
  std::optional<Spacing> spacing;
  std::optional<Flex> flex;
  std::optional<PositionType> positionType;
  std::optional<Position> position;
  std::optional<Display> display;
  std::optional<Overflow> overflow;

  bool operator<=>(const Layout&) const = default;
};

}  // namespace style
}  // namespace ui