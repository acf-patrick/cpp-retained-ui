#pragma once

#include <optional>
#include "./Display.h"
#include "./Flex.h"
#include "./Overflow.h"
#include "./PositionType.h"
#include "./Size.h"
#include "./Spacing.h"

namespace ui {
namespace style {

struct Style {
  std::optional<Size> size;
  std::optional<Spacing> spacing;
  std::optional<Flex> flex;
  std::optional<PositionType> position;
  std::optional<Display> display;
  std::optional<Overflow> overflow;
};

}  // namespace style
}  // namespace ui