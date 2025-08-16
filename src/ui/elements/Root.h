#pragma once

#include "./Element.h"
#include <raylib.h>

namespace ui {
namespace element {

class Root : public Element {
  YGConfigRef _config;

 public:
  Root(const Vector2& windowSize);
  ~Root();
  
  void render() override;
// TODO : create dirty flags
  void calculateLayout();
};

}  // namespace element
}  // namespace ui