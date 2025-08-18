#include "./Stack.h"

namespace ui {
namespace element {

Stack::Stack() : Element("Stack") {
  auto layout = _layout;
  auto& flex = layout.flex.emplace();
  flex.flexDirection = ui::style::FlexDirection::Column;
  updateLayout(layout);
}

}  // namespace element
}  // namespace ui