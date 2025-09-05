#pragma once

#include <raylib.h>
#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "../styles/Layout.h"
#include "../styles/Style.h"
#include "../styles/Theme.h"

namespace ui {

namespace rendering { // forward declarations
class StackingContext;
} // namespace rendering

namespace element {

/**
 * Instantiating this class on its own without using `std::shared_ptr` and `std::mkae_shared`
 * potentially throws an exception or undefined behavior. 
*/
class Element : public std::enable_shared_from_this<Element> {
  public:
    using ElementId = unsigned int;

  private:
    static ElementId nextId;

  protected:
    ui::style::Layout _layout;
    ui::style::Style _style;
    YGNodeRef _yogaNode;
    ElementId _id;
    ui::style::Theme _preferredTheme;
    std::string _name;         // name of this element
    Vector2 _absolutePosition; // relative to root element
    std::weak_ptr<Element> _parent;
    std::weak_ptr<ui::rendering::StackingContext> _stackingContext;
    std::vector<std::shared_ptr<Element>> _children;
    bool _dirtyCachedInheritableProps;
    ui::style::Inheritables _cachedInheritableProps;

  protected:
    Element(const std::string &name = "Element");

    void updateBoxSizing(ui::style::BoxSizing boxSizing);
    void updateOverflow(ui::style::Overflow overflow);
    void updateDisplay(ui::style::Display display);
    void updatePosition(const ui::style::Position &position);
    void updatePositionType(ui::style::PositionType positionType);
    void updateFlex(const ui::style::Flex &flex);
    void updateSpacing(const ui::style::Spacing &spacing);
    void updateSize(const ui::style::Size &size);
    void updateCachedInheritablePropsFrom(std::shared_ptr<Element> element);

  protected:
    // used by AppendChild methods
    void setParent(std::shared_ptr<Element> parent);

    void setPreferredTheme(ui::style::Theme theme);

    int getSegmentCount(float radius) const;
    void markInheritableStylesAsDirty();
    void markLayoutAsDirty();

    virtual void onPreferredThemeChanged(ui::style::Theme theme);
    virtual void onChildAppended(std::shared_ptr<Element> child);
    virtual void onStyleDirtyFlagTriggered();
    virtual void onLayoutDirtyFlagTriggered();

  protected:
    void drawBackground(const Rectangle &rect);
    void drawBorder(const Rectangle &rect);

  public:
    virtual ~Element();

    // Custom implementation for inherited classes should only render themselves not their children.
    virtual void render();

    void removeChild(std::shared_ptr<Element> child);

    void removeAllChildren();

    // Dirty flag won't be broadcast if this element does not have a parent element.
    // Atempting to update inner node directly instead of calling this function will cause
    // undefined behavior in dirty layout detection
    void updateLayout(const ui::style::Layout &layout);

    // Atempting to set _style property directly instead of calling this function
    // will cause undefined behavior in inherited properties update
    void updateStyle(const ui::style::Style &style);

    bool hasItsOwnStackingContext() const;

    // return display == none
    bool isNotDisplayed() const;

    bool isRoot() const;

    // Warning : must be called after layout calculation. Compute absolute
    // position of this element from parent's absolute position
    void updateAbsolutePosition();

    std::vector<std::shared_ptr<Element>> getSiblings();

    ElementId getId() const;

    std::shared_ptr<ui::rendering::StackingContext> getParentStackingContext() const;

    void setStackingContext(std::shared_ptr<ui::rendering::StackingContext> ctx);
    std::shared_ptr<ui::rendering::StackingContext> getStackingContext() const;

    ui::style::Layout getLayout() const;

    ui::style::Style getStyle() const;

    std::shared_ptr<Element> getParent() const;

    std::vector<std::shared_ptr<Element>> getChildren();

    // Get bouding box of current element. (absolutePosition + yoga size)
    // Might be invalid if called before layout calculation
    Rectangle getBoundingRect() const;

    Vector2 getPosition() const;
    Vector2 getSize() const;

    bool contains(const Vector2 &point) const;

    ui::style::Theme getPreferredTheme() const;

    Element& appendChild(std::shared_ptr<Element> child);

    template <typename... Elements>
    void appendChildren(std::shared_ptr<Elements> &&...children) {
        (appendChild(children), ...);
    }

    friend class Root;
};

} // namespace element
} // namespace ui