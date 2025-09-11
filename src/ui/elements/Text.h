#pragma once

#include <string>
#include <optional>

#include <raylib.h>

#include "./Element.h"

namespace ui {
namespace element {

class Text : public Element {
  std::string _text;

  std::optional<Font> getUsedFont() const;

  void onChildAppended(std::shared_ptr<Element>) override;

 public:
  Text(const std::string& text);
  void setText(const std::string& text);
  void render(const Vector2&) override;
};

}  // namespace element
}  // namespace ui