#include "./StackingContext.h"
#include "../elements/Element.h"
#include "./Layer.h"

#include <algorithm>

ui::rendering::StackingContext::StackingContextId ui::rendering::StackingContext::nextId = 0;

namespace ui {
namespace rendering {

StackingContext::Context::Context(const ui::style::Style &style) {
    opacity = style.opacity;
    zIndex = style.zIndex;
    transform = style.transform;
}

StackingContext::StackingContext(std::shared_ptr<ui::element::Element> owner) : _owner(owner) {
    if (!owner) {
        const std::string errorMessage("[StackingContext] stacking context created on null element");
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::runtime_error(errorMessage);
    }

    _id = nextId++;
}

StackingContext::~StackingContext() {}

std::vector<std::shared_ptr<StackingContext>> StackingContext::getChildren() const {
    return std::vector(_children.begin(), _children.end());
}

StackingContext::Context StackingContext::getContext() const {
    if (auto owner = _owner.lock())
        return Context(owner->getStyle());

    TraceLog(LOG_ERROR, "[StackingContext] %d has null owner element", _id);
    throw std::runtime_error("[StackingContext] Null owner");
}

std::shared_ptr<StackingContext> StackingContext::getParent() const {
    return _parent.lock();
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
    if (!owner) {
        const std::string errorMessage("[StackingContext] nullptr provided as owner");
        TraceLog(LOG_ERROR, errorMessage.c_str());
    } else
        _owner = owner;
}

void StackingContext::setLayer(std::shared_ptr<Layer> layer) {
    _layer = layer;
}

std::shared_ptr<Layer> StackingContext::getLayer() const {
    return _layer.lock();
}

std::shared_ptr<Layer> StackingContext::getParentLayer() const {
    if (auto parent = _parent.lock())
        return parent->getLayer();
    return nullptr;
}

void StackingContext::render() {
    auto layer = getLayer();
    if (!layer) {
        TraceLog(LOG_ERROR, "[StackingContext] %d does not have a layer", _id);
        return;
    }

    auto owner = getOwner();
    if (!owner) {
        TraceLog(LOG_ERROR, "[StackingContext] %d does not have an owner element", _id);
        return;
    }

    if (!layer->clean())
        layer->clearRenderTarget();

    auto useLayerGuard = layer->use();
    owner->render();
    for (auto e : _elements) {
        if (auto element = e.lock())
            element->render();
    }

    // TODO : mark layer as dirty
}

std::shared_ptr<StackingContext> StackingContext::AppendChild(std::shared_ptr<StackingContext> parent, std::shared_ptr<StackingContext> child) {
    if (parent == nullptr || child == nullptr)
        return parent;

    parent->appendChild(child);
    child->setParent(parent);

    return parent;
}

bool StackingContext::IsRequiredFor(std::shared_ptr<ui::element::Element> element) {
    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return element->isRoot() || style.opacity < 1.0f ||
           style.transform.has_value() ||
           style.zIndex != 0 || std::holds_alternative<ui::style::IsolationIsolate>(style.isolation);
}

} // namespace rendering
} // namespace ui