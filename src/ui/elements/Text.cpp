#include "./Text.h"
#include <raylib.h>
#include <yoga/YGNodeLayout.h>

namespace ui {
namespace element {

Text::Text(const std::string &text) : Element("Text"), _text(text) {
}

void Text::setText(const std::string &text) {
    _text = text;
    const auto fontSize = _cachedInheritableProps.fontSize.unwrap();
    const auto width = MeasureText(text.c_str(), fontSize);

    auto layout = getLayout();
    {
        auto &size = layout.size.emplace();
        size.width = utils::Value(width);
        size.height = utils::Value<int>(fontSize);
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
    DrawText(_text.c_str(), bb.x, bb.y, _cachedInheritableProps.fontSize.unwrap(), _cachedInheritableProps.color.unwrap());
}

void Text::onChildAppended(std::shared_ptr<Element>) {
    throw std::logic_error("Text element can only be used as leaf node.");
}

} // namespace element
} // namespace ui