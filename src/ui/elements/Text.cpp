#include "./Text.h"
#include "../../core/repository/FontRepository.h"

#include <raylib.h>
#include <yoga/YGNodeLayout.h>

namespace ui {
namespace element {

Text::Text(const std::string &text) : Element("Text"), _text(text) {}

void Text::setText(const std::string &text) {
    _text = text;
    const auto fontSize = _cachedInheritableProps.fontSize.unwrap();
    Vector2 textSize;

    if (auto font = getUsedFont())
        textSize = MeasureTextEx(*font, text.c_str(), fontSize, _cachedInheritableProps.letterSpacing.unwrap());
    else {
        textSize.x = MeasureText(text.c_str(), fontSize);
        textSize.y = fontSize;
    }

    auto layout = getLayout();
    {
        auto &size = layout.size.emplace();
        size.width = utils::Value<int>(textSize.x);
        size.height = utils::Value<int>(textSize.y);
    }
    updateLayout(layout);
}

void Text::render() {
    static bool firstRender = true;
    if (firstRender) {
        firstRender = false;
        setText(_text);
    }

    auto bb = getBoundingRect();
    const auto fontSize = _cachedInheritableProps.fontSize.unwrap();
    const auto color = _cachedInheritableProps.color.unwrap();

    if (auto font = getUsedFont())
        DrawTextEx(*font, _text.c_str(), {bb.x, bb.y}, fontSize, _cachedInheritableProps.letterSpacing.unwrap(), color);
    else
        DrawText(_text.c_str(), bb.x, bb.y, fontSize, color);
}

std::optional<Font> Text::getUsedFont() const {
    auto fonts = repository::FontRepository::Get();
    if (!fonts) {
        const std::string errorMessage("[Text] Font repository not initialized.");
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }

    const auto fontFamily = _cachedInheritableProps.fontFamily.unwrap();
    std::optional<Font> font;

    for (const auto &fontName : fontFamily) {
        if (auto registeredFont = fonts->get(fontName)) {
            font = registeredFont;
            break;
        }
    }

    return font;
}

void Text::onChildAppended(std::shared_ptr<Element>) {
    const std::string errorMessage("[Text] Text element can only be used as leaf node.");
    TraceLog(LOG_FATAL, errorMessage.c_str());
    throw std::logic_error(errorMessage);
}

} // namespace element
} // namespace ui