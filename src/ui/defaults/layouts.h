#pragma once

#include "../styles/Layout.h"

struct Vector2;

namespace ui {
namespace defaults {

ui::style::Layout elementLayout();
ui::style::Layout rootLayout(const Vector2& windowSize);
ui::style::Layout buttonLayout();

}
}  // namespace ui