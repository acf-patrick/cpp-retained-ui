#include "./Text.h"
#include <raylib.h>
#include <yoga/YGNodeLayout.h>
#include <iostream>

namespace ui {
namespace element {

Text::Text(const std::string& text) {
  setText(text);
}

void Text::setText(const std::string& text) {
  _text = text;
  const auto width = MeasureText(text.c_str(), 16);
  YGNodeStyleSetWidth(_yogaNode, width);
  YGNodeStyleSetHeight(_yogaNode, 16);
}

void Text::render() {
  auto bb = getBoundingRect();
  DrawText(_text.c_str(), bb.x, bb.y, 16, WHITE);
}

}  // namespace element
}  // namespace ui