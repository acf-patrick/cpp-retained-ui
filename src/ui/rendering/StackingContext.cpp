#include "./StackingContext.h"
#include "../elements/Element.h"
#include "../styles/Style.h"

#include <algorithm>

ui::rendering::StackingContext::StackingContextId ui::rendering::StackingContext::nextId = 0;

namespace ui {
namespace rendering {

StackingContext::Context::Context(const ui::style::Style &style) {
    opacity = style.opacity;
    zIndex = style.zIndex;
}

StackingContext::StackingContext(std::shared_ptr<ui::element::Element> owner, std::shared_ptr<StackingContext> parent) : _parent(parent), _owner(owner) {
    if (!owner) {
        const std::string errorMessage("[StackingContext] stacking context created on null element");
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::runtime_error(errorMessage);
    }

    _context = owner->getStyle();
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
        _elements.erase(
            std::remove_if(_elements.begin(), _elements.end(),
                           [=](std::weak_ptr<ui::element::Element> e) { return e.lock() == element; }));
}

std::shared_ptr<ui::element::Element> StackingContext::getOwner() const {
    return _owner.lock();
}

void StackingContext::setOwner(std::shared_ptr<ui::element::Element> owner) {
    _owner = owner;
}

std::shared_ptr<StackingContext> StackingContext::AppendChild(std::shared_ptr<StackingContext> parent, std::shared_ptr<StackingContext> child) {
    if (parent == nullptr || child == nullptr)
        return parent;

    parent->appendChild(child);
    child->setParent(parent);

    return parent;
}

bool StackingContext::IsRequiredFor(std::shared_ptr<ui::element::Element> element) {
    const auto style = element->getStyle();
    return style.opacity < 1.0f ||
           style.zIndex != 0 || std::holds_alternative<ui::style::IsolationIsolate>(style.isolation);
}

} // namespace rendering
} // namespace ui