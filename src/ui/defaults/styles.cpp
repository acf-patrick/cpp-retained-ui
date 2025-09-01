#include "./styles.h"
#include <raylib.h>

namespace ui {
namespace defaults {

ui::style::Style elementStyles(ui::style::Theme theme) {
    ui::style::Style style;
    style.opacity = 1.0;
    style.zIndex = 0;
    style.isolation = ui::style::IsolationAuto{};
    return style;
}

ui::style::Style rootStyles(ui::style::Theme theme) {
    auto style = elementStyles(theme);

    style.inheritables.color = theme == ui::style::Theme::Dark ? WHITE : BLACK;
    style.inheritables.fontSize = 16;
    style.inheritables.fontFamily = std::vector<std::string>();
    style.inheritables.letterSpacing = 0;

    style.backgroundColor = theme == ui::style::Theme::Dark ? BLACK : WHITE;

    return style;
}

ui::style::Style buttonStyles(ui::style::Theme theme) {
    auto style = elementStyles(theme);

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

ui::style::Style imageStyles() {
    auto style = elementStyles(ui::style::Theme::Dark);

    auto &props = style.drawableContentProps.emplace();
    props.objectFit = ui::style::ObjectFit::Fill;
    props.objectPosition = ui::style::ObjectPositionCenter{};

    return style;
}

} // namespace defaults
} // namespace ui