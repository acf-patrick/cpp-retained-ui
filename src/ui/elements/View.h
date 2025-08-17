#pragma once

#include "./Element.h"
#include <iostream>

namespace ui {
namespace element {

class View : public Element {
 public:
  View() : Element("View") {}
};

}  // namespace element
}  // namespace ui