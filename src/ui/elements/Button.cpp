#include "./Button.h"
#include "../defaults.h"

namespace ui {
namespace element {

Button::Button() : Element("Button") {
    updateLayout(ui::defaults::buttonLayout());
    updateStyle(ui::defaults::buttonStyles(_preferredTheme));
}

void Button::onPreferredThemeChanged(ui::style::Theme theme) {
    updateStyle(ui::defaults::buttonStyles(theme));
}

} // namespace element
} // namespace ui