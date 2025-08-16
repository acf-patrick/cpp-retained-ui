#pragma once

#include <string>
#include "./Element.h"

namespace ui {
namespace element {

class Text : public Element {
    std::string _text;

 public:
  Text(const std::string& text);
  void setText(const std::string& text);
  void render() override;
};

}  // namespace element
}  // namespace ui