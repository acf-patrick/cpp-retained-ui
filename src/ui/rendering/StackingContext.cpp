#include "./StackingContext.h"
#include "../elements/Element.h"
#include "./Layer.h"

#include <algorithm>
#include <queue>
#include <ranges>

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
    return _children;
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

void StackingContext::sortChildrenByZIndex() {
    std::sort(
        _children.begin(),
        _children.end(),
        [](const std::shared_ptr<StackingContext> &ctxA, const std::shared_ptr<StackingContext> &ctxB) {
            return ctxA->getContext().zIndex >= ctxB->getContext().zIndex;
        });
}

void StackingContext::repositionInParent() {
    if (auto parent = _parent.lock())
        parent->sortChildrenByZIndex();
}

void StackingContext::appendChild(std::shared_ptr<StackingContext> child) {
    if (!child)
        return;

    _children.push_back(child);
    sortChildrenByZIndex();
    child->setParent(shared_from_this());
}

void StackingContext::removeChild(std::shared_ptr<StackingContext> child) {
    if (child) {
        _children.erase(std::remove(_children.begin(), _children.end(), child));
        sortChildrenByZIndex();
    }
}

void StackingContext::replaceChild(std::shared_ptr<StackingContext> oldCtx, std::shared_ptr<StackingContext> newCtx) {
    std::replace(_children.begin(), _children.end(), oldCtx, newCtx);
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

bool StackingContext::needsItsOwnLayer() const {
    if (auto owner = _owner.lock())
        return Layer::IsRequiredFor(owner);

    return false;
}

bool StackingContext::hasItsOwnLayer() const {
    return _layer.lock() != nullptr;
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

    if (!layer->isClean())
        layer->clearRenderTarget();

    auto useLayerGuard = layer->use();
    if (!owner->isNotDisplayed()) {
        owner->render();

        for (auto e : _elements) {
            if (auto element = e.lock())
                if (!element->isNotDisplayed())
                    element->render();
        }
    }
}

std::vector<std::shared_ptr<ui::element::Element>> StackingContext::getElements() const {
    std::vector<std::shared_ptr<ui::element::Element>> elements(_elements.size());
    std::transform(_elements.begin(), _elements.end(), elements.begin(),
                   [](const std::weak_ptr<ui::element::Element> &element) { return element.lock(); });
    return elements;
}

void StackingContext::revokeElements(const std::set<std::shared_ptr<ui::element::Element>> &revokedElements) {
    auto elements = getElements();

    std::vector<std::shared_ptr<ui::element::Element>> remainingElements;
    std::copy_if(
        elements.begin(), elements.end(),
        std::back_inserter(remainingElements),
        [revokedElements](const std::shared_ptr<ui::element::Element> &element) { return !revokedElements.contains(element); });

    _elements.resize(remainingElements.size());
    int i = 0;
    for (auto e : remainingElements)
        _elements[i++] = e;
}

bool StackingContext::IsRequiredFor(std::shared_ptr<const ui::element::Element> element) {
    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return element->isRoot() || style.opacity < 1.0f ||
           style.transform.has_value() ||
           style.zIndex != 0 || std::holds_alternative<ui::style::IsolationIsolate>(style.isolation);
}

std::shared_ptr<StackingContext> StackingContext::BuildTree(std::shared_ptr<ui::element::Element> elementsRoot) {
    // [currentElement, parentCtx]
    std::queue<std::pair<std::shared_ptr<ui::element::Element>, std::shared_ptr<StackingContext>>> queue;
    std::shared_ptr<ui::rendering::StackingContext> rootCtx;

    queue.push({elementsRoot, nullptr});
    while (!queue.empty()) {
        auto [e, parentCtx] = queue.front();
        queue.pop();

        if (ui::rendering::StackingContext::IsRequiredFor(e)) {
            auto ctx = std::make_shared<ui::rendering::StackingContext>(e);
            e->setStackingContext(ctx);
            if (parentCtx)
                parentCtx->appendChild(ctx);

            if (e == elementsRoot)
                rootCtx = ctx;
        } else {
            parentCtx->addElement(e);
            e->setStackingContext(parentCtx);
        }

        for (auto child : e->getChildren())
            queue.push({child, e->getStackingContext()});
    }

    return rootCtx;
}

} // namespace rendering
} // namespace ui