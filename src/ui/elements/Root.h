#pragma once

#include "./Element.h"
#include <raylib.h>

#include <memory>

namespace ui {
namespace element {

class Root : public Element {
    YGConfigRef _config;
    bool _finalized = false;
    bool _dirtyLayout = true; // should calculate layout at least once

    void calculateLayout();
    void propagateStyles();
    void propagatePreferredTheme();

  private:
    void onLayoutDirtyFlagTriggered() override;
    void onPreferredThemeChanged(ui::style::Theme theme) override;

  public:
    Root(const Vector2 &windowSize);
    ~Root();

    // check for styles and layout update
    void update();

    void render() override;

    // Construct UI tree
    void finalize();
};

} // namespace element
} // namespace ui