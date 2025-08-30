#pragma once

#include <memory>
#include <vector>

namespace ui {

namespace element {
class Element; // forward declaration
}

namespace rendering {

class StackingContext {
  public:
    using StackingContextId = unsigned int;

    struct Context {
        float opacity; // ]0; 1[
        int zIndex;    // > 0
    };

  private:
    static StackingContextId nextId;

    Context _context;
    std::weak_ptr<StackingContext> _parent;
    std::vector<std::shared_ptr<StackingContext>> _children;
    std::vector<std::shared_ptr<ui::element::Element>> _elements;
    StackingContextId _id;

    void appendChild(std::shared_ptr<StackingContext> child);

  public:
    StackingContext(const Context &ctx, std::shared_ptr<StackingContext> parent = nullptr);
    ~StackingContext();

    std::vector<std::shared_ptr<StackingContext>> getChildren() const;

    void setParent(std::shared_ptr<StackingContext> parent);
    void removeChild(std::shared_ptr<StackingContext> child);

    void addElement(std::shared_ptr<ui::element::Element> element);
    void removeElement(std::shared_ptr<ui::element::Element> element);

    static std::shared_ptr<StackingContext> AppendChild(std::shared_ptr<StackingContext> parent, std::shared_ptr<StackingContext> child);

    template <typename... Contexts>
    static std::shared_ptr<StackingContext> AppendChildren(std::shared_ptr<StackingContext> parent, std::shared_ptr<Contexts> &&...contexts) {
        (AppendChild(parent, contexts), ...);
        return parent;
    }
};

} // namespace rendering
} // namespace ui
