#pragma once

#include <yoga/YGNodeLayout.h>
#include "./Element.h"

#include <raylib.h>

namespace ui {
namespace element {

class Button : public Element {
 public:
  Button() : Element("Button") {}
};

}  // namespace element
}  // namespace ui