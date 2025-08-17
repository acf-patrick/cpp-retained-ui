#pragma once

#include "./Element.h"
#include <raylib.h>

namespace ui {
namespace element {

class Root : public Element {
  YGConfigRef _config;
  bool _ok = false;
  bool _dirty = true; // should calculate layout at least once

void propagatePreferredTheme();

  void onDirtyFlagTriggered() override;
  void onPreferredThemeChanged(ui::style::Theme theme) override;

 public:
  Root(const Vector2& windowSize);
  ~Root();
  
  void render() override;

  bool shouldRecalculateLayout() const;

  // Construct UI tree
  void finalize();

  void calculateLayout();
};

}  // namespace element
}  // namespace ui