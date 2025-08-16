#pragma once

#include <raylib.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>

#include <memory>
#include <optional>
#include <vector>

#include "../styles/Style.h"

namespace ui {
namespace element {

class Element {
  enum UpdateStyleType : unsigned int {
    None = 0,
    Display = 1 << 1,
    Position = 1 << 2,
    Flex = 1 << 3,
    Spacing = 1 << 4,
    Size = 1 << 5,    
    Overflow = 1 << 6,
    PositionType = 1 << 7
  };

  static unsigned int nextId;

 protected:
  ui::style::Style _style;
  YGNodeRef _yogaNode;
  unsigned int _id;
  Vector2 _absolutePosition;  // relative to root element
  std::weak_ptr<Element> _parent;
  std::vector<std::shared_ptr<Element>> _children;

 protected:
  void updateOverflow(ui::style::Overflow overflow);
  void updateDisplay(ui::style::Display display);
  void updatePosition(const ui::style::Position& position);
  void updatePositionType(ui::style::PositionType positionType);
  void updateFlex(const ui::style::Flex& flex);
  void updateSpacing(const ui::style::Spacing& spacing);
  void updateSize(const ui::style::Size& size);

 protected:
  void appendChild(std::shared_ptr<Element> child);
  bool shouldTriggerDirtyFlag(int updateType) const;
  virtual void onDirtyFlagChanged();
  void markAsDirty();

 public:
  Element();
  virtual ~Element();

  virtual void render() = 0;

  void removeChild(std::shared_ptr<Element> child);

  void removeAllChildren();

  void updateStyle(const ui::style::Style& style);

  // return display == none
  bool isNotDisplayed() const;

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