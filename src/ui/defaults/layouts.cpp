#include "./layouts.h"

#include <raylib.h>

namespace ui {
namespace defaults {

ui::style::Layout elementLayout() {
  ui::style::Layout layout;

  layout.boxSizing = ui::style::BoxSizing::ContentBox;

  return layout;
}

ui::style::Layout rootLayout(const Vector2& windowSize) {
  ui::style::Layout layout;
  auto& size = layout.size.emplace();

  size.width = utils::Value<int>(windowSize.x);
  size.height = utils::Value<int>(windowSize.y);

  return layout;
}

ui::style::Layout buttonLayout() {
  ui::style::Layout layout;

  auto& spacing = layout.spacing.emplace();
  spacing.paddingHorizontal = utils::Value(10);
  spacing.paddingVertical = utils::Value(5);
  spacing.border = 2;

  return layout;
}

}  // namespace defaults
}  // namespace ui