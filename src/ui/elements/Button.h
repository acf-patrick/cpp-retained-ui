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
    const auto bb = getBoundingRect();
    DrawRectangle(bb.x, bb.y, bb.width, bb.height, _color);
  }
};

}  // namespace element
}  // namespace ui