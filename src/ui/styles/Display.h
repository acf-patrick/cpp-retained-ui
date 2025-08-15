#pragma once

namespace ui {
namespace style {

enum class Display {
  Flex,
  None,
  Contents  // remove from the layout flow, while preserving and hoisting the
            // node's children
};

}
}  // namespace ui
