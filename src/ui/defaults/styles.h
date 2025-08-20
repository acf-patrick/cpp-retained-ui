#pragma once

#include "../styles/Style.h"
#include "../styles/Theme.h"

namespace ui {
namespace defaults {

ui::style::Style elementStyles(ui::style::Theme theme);
ui::style::Style rootStyles(ui::style::Theme theme);
ui::style::Style buttonStyles(ui::style::Theme theme);
ui::style::Style imageStyles();

}  // namespace styles
}  // namespace ui