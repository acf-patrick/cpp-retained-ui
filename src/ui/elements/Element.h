#pragma once

#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>

#include <memory>
#include <optional>
#include <vector>

#include "../styles/Style.h"
#include "../../utils/functions.h"

namespace ui {
namespace element {

class Element {
  ui::style::Style _style;
  static unsigned int nextId;

 protected:
  YGNodeRef _yogaNode;
  unsigned int _id;
  std::vector<std::shared_ptr<Element>> _children;

  void updateOverflow(ui::style::Overflow overflow);
  void updateDisplay(ui::style::Display display);
  void updatePosition(ui::style::PositionType position);
  void updateFlex(const ui::style::Flex& flex);
  void updateSpacing(const ui::style::Spacing& spacing);
  void updateSize(const ui::style::Size& size);

 public:
  Element();
  virtual ~Element();

  virtual void render() = 0;

  void appendChild(std::shared_ptr<Element> child);

  void removeChild(std::shared_ptr<Element> child);

  void removeAllChildren();

  void updateStyle(const ui::style::Style& style);

  unsigned int getId() const;

  ui::style::Style getStyle() const;

  std::vector<std::shared_ptr<Element>> getChildren();
};

}  // namespace element
}  // namespace ui