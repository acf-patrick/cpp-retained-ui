#pragma once

#include <memory>
#include <vector>

namespace ui {

namespace style {
class Style; // forward declaration
}

namespace element {
class Element; // forward declaration
}

namespace rendering {

class Layer; //forward declaration

class StackingContext {
  public:
    using StackingContextId = unsigned int;

    struct Context {
        float opacity; // ]0; 1[
        int zIndex;    // > 0

        Context() = default;
        Context(const ui::style::Style &style);
    };

  private:
    static StackingContextId nextId;

    Context _context;
    std::weak_ptr<StackingContext> _parent;
    std::weak_ptr<ui::element::Element> _owner;
    std::vector<std::shared_ptr<StackingContext>> _children;
    std::vector<std::weak_ptr<ui::element::Element>> _elements;
    std::weak_ptr<Layer> _layer; // optional layer if GPU surface is required
    StackingContextId _id;

    void appendChild(std::shared_ptr<StackingContext> child);

  public:
    StackingContext(std::shared_ptr<ui::element::Element> owner, std::shared_ptr<StackingContext> parent = nullptr);
    ~StackingContext();

    std::shared_ptr<ui::element::Element> getOwner() const;
    void setOwner(std::shared_ptr<ui::element::Element> owner);

    std::vector<std::shared_ptr<StackingContext>> getChildren() const;

    void setParent(std::shared_ptr<StackingContext> parent);
    void removeChild(std::shared_ptr<StackingContext> child);

    void addElement(std::shared_ptr<ui::element::Element> element);
    void removeElement(std::shared_ptr<ui::element::Element> element);

    static bool IsRequiredFor(std::shared_ptr<ui::element::Element> element);

    static std::shared_ptr<StackingContext> AppendChild(std::shared_ptr<StackingContext> parent, std::shared_ptr<StackingContext> child);

    template <typename... Contexts>
    static std::shared_ptr<StackingContext> AppendChildren(std::shared_ptr<StackingContext> parent, std::shared_ptr<Contexts> &&...contexts) {
        (AppendChild(parent, contexts), ...);
        return parent;
    }
};

} // namespace rendering
} // namespace ui
