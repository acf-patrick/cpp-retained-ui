#pragma once
#include "./Element.h"
#include <iostream>

namespace ui {
namespace element {

class Button : public Element {
    void onPreferredThemeChanged(ui::style::Theme theme) override;

  public:
    Button();
};

} // namespace element
} // namespace ui