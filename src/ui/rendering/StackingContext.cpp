#include "./StackingContext.h"

#include <algorithm>

ui::rendering::StackingContext::StackingContextId ui::rendering::StackingContext::nextId = 0;

namespace ui {
namespace rendering {

StackingContext::StackingContext(const StackingContext::Context &ctx, std::shared_ptr<StackingContext> parent) : _context(ctx), _parent(parent) {
    _id = nextId++;
}

StackingContext::~StackingContext() {}

std::vector<std::shared_ptr<StackingContext>> StackingContext::getChildren() const {
    return _children;
}

void StackingContext::setParent(std::shared_ptr<StackingContext> parent) {
    _parent = parent;
}

void StackingContext::appendChild(std::shared_ptr<StackingContext> child) {
    if (child)
        _children.push_back(child);
}

void StackingContext::removeChild(std::shared_ptr<StackingContext> child) {
    if (child)
        _children.erase(std::remove(_children.begin(), _children.end(), child));
}

void StackingContext::addElement(std::shared_ptr<ui::element::Element> element) {
    if (element)
        _elements.push_back(element);
}
void StackingContext::removeElement(std::shared_ptr<ui::element::Element> element) {
    if (element)
        _elements.erase(std::remove(_elements.begin(), _elements.end(), element));
}

std::shared_ptr<StackingContext> StackingContext::AppendChild(std::shared_ptr<StackingContext> parent, std::shared_ptr<StackingContext> child) {
    if (parent == nullptr || child == nullptr)
        return parent;

    parent->appendChild(child);
    child->setParent(parent);

    return parent;
}

} // namespace rendering
} // namespace ui