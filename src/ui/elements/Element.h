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
namespace element {

class Element {
    static unsigned int nextId;

  protected:
    ui::style::Layout _layout;
    ui::style::Style _style;
    YGNodeRef _yogaNode;
    unsigned int _id;
    ui::style::Theme _preferredTheme;
    std::string _name;         // name of this element
    Vector2 _absolutePosition; // relative to root element
    std::weak_ptr<Element> _parent;
    std::vector<std::shared_ptr<Element>> _children;
    ui::style::Inheritables _cachedInheritableProps;
    std::optional<Color> _cachedColor; // cached color on this node

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
    void appendChild(std::shared_ptr<Element> child);

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

    // Layout rendering is performed on root node using BFS.
    // Custom implementation for inherited classes should only render themselves
    // not their children.
    virtual void render();

    void removeChild(std::shared_ptr<Element> child);

    void removeAllChildren();

    // Dirty flag won't be broadcast if this element does not have a parent
    // element
    void updateLayout(const ui::style::Layout &layout);

    void updateStyle(const ui::style::Style &style);

    // return display == none
    bool isNotDisplayed() const;

    // Warning : must be called after layout calculation. Compute absolute
    // position of this element from parent's absolute position
    void updateAbsolutePosition();

    std::vector<std::shared_ptr<Element>> getSiblings();

    unsigned int getId() const;

    ui::style::Layout getLayout() const;

    ui::style::Style getStyle() const;

    std::shared_ptr<Element> getParent();

    std::vector<std::shared_ptr<Element>> getChildren();

    // Get bouding box of current element.
    // Might be invalid if called before layout calculation
    Rectangle getBoundingRect() const;

    ui::style::Theme getPreferredTheme() const;

    // Append child to parent's children and set child's parent.
    // The reason behind this function instead of a method is to keep owned
    // reference to both parent and child element by deciding to wrap parent with
    // weak_ptr
    // @return parent element
    static std::shared_ptr<Element> AppendChild(std::shared_ptr<Element> parent,
                                                std::shared_ptr<Element> child);

    template <typename... Elements>
    static std::shared_ptr<Element> AppendChildren(
        std::shared_ptr<Element> parent,
        Elements &&...children) {
        (AppendChild(parent, children), ...);
        return parent;
    }

    friend class Root;
};

} // namespace element
} // namespace ui