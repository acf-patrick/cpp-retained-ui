#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "../styles/Transform.h"

namespace ui {

namespace style { // forward declaration
struct Style;
}

namespace element {
class Element; // forward declaration
}

namespace rendering {

class Layer; // forward declaration

class StackingContext : public std::enable_shared_from_this<StackingContext> {
  public:
    using StackingContextId = unsigned int;

    struct Context {
        float opacity; // ]0; 1[
        int zIndex;    // > 0
        std::optional<ui::style::Transform> transform;

        Context() = default;
        Context(const ui::style::Style &style);

        bool operator<=>(const Context&) const = default;
    };

  private:
    static StackingContextId nextId;
    std::weak_ptr<StackingContext> _parent;
    std::weak_ptr<ui::element::Element> _owner;
    std::vector<std::shared_ptr<StackingContext>> _children;
    std::vector<std::weak_ptr<ui::element::Element>> _elements;
    std::weak_ptr<Layer> _layer; // optional layer if GPU surface is required
    StackingContextId _id;

    void sortChildrenByZIndex();

  public:
    StackingContext(std::shared_ptr<ui::element::Element> owner);
    ~StackingContext();

    std::shared_ptr<ui::element::Element> getOwner() const;
    void setOwner(std::shared_ptr<ui::element::Element> owner);

    std::vector<std::shared_ptr<StackingContext>> getChildren() const;

    // Render self and children in stack way
    void render();

    Context getContext() const;
    StackingContextId getId() const;

    void setLayer(std::shared_ptr<Layer> layer);
    std::shared_ptr<Layer> getParentLayer() const;
    std::shared_ptr<Layer> getLayer() const;

    std::shared_ptr<StackingContext> getParent() const;
    void setParent(std::shared_ptr<StackingContext> parent);
    void removeChild(std::shared_ptr<StackingContext> child);

    // Returns elements on this stacking context
    std::vector<std::shared_ptr<ui::element::Element>> getElements() const;


    bool hasItsOwnLayer() const;
    bool needsItsOwnLayer() const;

    // Sort parent's children
    void repositionInParent();

    void addElement(std::shared_ptr<ui::element::Element> element);
    void removeElement(std::shared_ptr<ui::element::Element> element);
    void removeElements(const std::vector<std::shared_ptr<ui::element::Element>>& elements);
    void replaceChild(std::shared_ptr<StackingContext> oldCtx, std::shared_ptr<StackingContext> newCtx);

    void takeOwnershipOfElements(std::shared_ptr<StackingContext> ctx);

    void appendChild(std::shared_ptr<StackingContext> child);

    template <typename... Contexts>
    void appendChildren(std::shared_ptr<Contexts> &&...contexts) {
        (appendChild(contexts), ...);
    }

    static std::shared_ptr<StackingContext> BuildTree(std::shared_ptr<ui::element::Element> element);

    static bool IsRequiredFor(std::shared_ptr<const ui::element::Element> element);
};

} // namespace rendering
} // namespace ui
