#include "./Text.h"
#include <iostream>
#include <raylib.h>
#include <yoga/YGNodeLayout.h>

namespace ui {
namespace element {

Text::Text(const std::string &text) : Element("Text") {
    setText(text);
}

void Text::setText(const std::string &text) {
    _text = text;
    const auto width = MeasureText(text.c_str(), 16);
    YGNodeStyleSetWidth(_yogaNode, width);
    YGNodeStyleSetHeight(_yogaNode, 16);
}

void Text::render() {
    auto bb = getBoundingRect();
    DrawText(_text.c_str(), bb.x, bb.y, 16, _cachedInheritableProps.color.unwrap());
}

void Text::onChildAppended(std::shared_ptr<Element>) {
    throw std::logic_error("Text element can only be used as leaf node.");
}

} // namespace element
} // namespace ui