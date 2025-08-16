#pragma once

#include "./Element.h"
#include <raylib.h>

namespace ui {
namespace element {

class Root : public Element {
  YGConfigRef _config;
  bool _dirty = true; // should calculate layout at least once

  void onDirtyFlagChanged() override;

 public:
  Root(const Vector2& windowSize);
  ~Root();
  
  void render() override;

  bool shouldRecalculateLayout() const;

  void calculateLayout();
};

}  // namespace element
}  // namespace ui