#pragma once

#include <raylib.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>

#include <memory>
#include <optional>
#include <vector>

#include "../../utils/functions.h"
#include "../styles/Style.h"

namespace ui {
namespace element {

class Element {
  static unsigned int nextId;

 protected:
  ui::style::Style _style;
  YGNodeRef _yogaNode;
  unsigned int _id;
  Vector2 _absolutePosition;  // relative to root element
  std::weak_ptr<Element> _parent;
  std::vector<std::shared_ptr<Element>> _children;

  void appendChild(std::shared_ptr<Element> child);

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

  void removeChild(std::shared_ptr<Element> child);

  void removeAllChildren();

  void updateStyle(const ui::style::Style& style);

  // Warning : must be called after layout calculation. Compute absolute
  // position of this element from parent's absolute position
  void updateAbsolutePosition();

  std::vector<std::shared_ptr<Element>> getSiblings();

  unsigned int getId() const;

  ui::style::Style getStyle() const;

  std::shared_ptr<Element> getParent();

  std::vector<std::shared_ptr<Element>> getChildren();

  // might be invalid if called before layout calculation
  Rectangle getBoundingRect() const;

  // append child to parent's children and set child's parent. Returns parent
  // element
  static std::shared_ptr<Element> AppendChild(std::shared_ptr<Element> parent,
                                              std::shared_ptr<Element> child);

  template <typename... Elements>
  static std::shared_ptr<Element> AppendChildren(
      std::shared_ptr<Element> parent,
      Elements&&... children) {
    (AppendChild(parent, children), ...);
    return parent;
  }
};

}  // namespace element
}  // namespace ui