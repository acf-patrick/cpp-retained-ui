#pragma once

#include <yoga/YGNodeLayout.h>
#include "./Element.h"

#include <raylib.h>

namespace ui {
namespace element {

class Button : public Element {
    Color _color;
 public:
  Button(const Color& color) { _color = color; }

  void render() override {
    float left = YGNodeLayoutGetLeft(_yogaNode);
    float top = YGNodeLayoutGetTop(_yogaNode);
    float width = YGNodeLayoutGetWidth(_yogaNode);
    float height = YGNodeLayoutGetHeight(_yogaNode);

    DrawRectangle(left, top, width, height, _color);
  }
};

}  // namespace element
}  // namespace ui