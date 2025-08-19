#include "./styles.h"
#include <raylib.h>

namespace ui {
namespace defaults {

ui::style::Style elementStyles(ui::style::Theme theme) {
    ui::style::Style style;
    return style;
}

ui::style::Style rootStyles(ui::style::Theme theme) {
    ui::style::Style style;

    style.inheritables.color = theme == ui::style::Theme::Dark ? WHITE : BLACK;
    style.inheritables.fontSize = 16;
    style.inheritables.fontFamily = std::vector<std::string>();
    style.inheritables.letterSpacing = 0;

    style.backgroundColor = theme == ui::style::Theme::Dark ? BLACK : WHITE;

    return style;
}

ui::style::Style buttonStyles(ui::style::Theme theme) {
    ui::style::Style style;

    switch (theme) {
    case ui::style::Theme::Dark: {
        style.backgroundColor = DARKPURPLE;
    } break;
    case ui::style::Theme::Light: {
        style.backgroundColor = LIGHTGRAY;
        style.borderColor = GRAY;
    } break;
    }

    return style;
}

} // namespace defaults
} // namespace ui