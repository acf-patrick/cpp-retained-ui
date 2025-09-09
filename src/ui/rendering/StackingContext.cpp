#include "./StackingContext.h"
#include "../elements/Element.h"
#include "./Layer.h"

#include <algorithm>
#include <format>
#include <queue>
#include <stack>
#include <set>

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

StackingContext::StackingContextId StackingContext::getId() const {
    return _id;
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
    auto owner = _owner.lock();
    if (!owner)
        return false;

    if (auto layer = _layer.lock())
        return layer->getOwner() == owner;

    return false;
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

    // TODO : render element relative to given layer

    auto useLayerGuard = layer->use();
    if (!owner->isNotDisplayed()) {
        owner->render();

        /*
        TODO : render from owner
        for (auto e : _elements) {
            if (auto element = e.lock())
                if (!element->isNotDisplayed())
                    element->render();
        }*/
    }
}

void StackingContext::renderTree() {
    std::stack<std::shared_ptr<StackingContext>> stack;
    stack.push(shared_from_this());

    while (!stack.empty()) {
        auto ctx = stack.top();
        stack.pop();

        ctx->render();
        for (auto child : ctx->getChildren())
            stack.push(child);
    }
}


std::vector<std::shared_ptr<ui::element::Element>> StackingContext::getElements() const {
    /*TODO */
}

void StackingContext::takeOwnershipOfElements(std::shared_ptr<StackingContext> ctx) {
    if (ctx == nullptr)
        return;

    if (std::find(_children.begin(), _children.end(), ctx) == _children.end()) {
        const auto errorMessage =
            std::format("[StackingContext] Can not take ownership of elements : provided context is not a direct child of the current context");
        TraceLog(LOG_ERROR, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }
/* TODO : set descendant of owner's stacking context to be this context
    bool processed = false;
    auto flattenedTree = ctx->getOwner()->flatten();
    std::vector<std::weak_ptr<ui::element::Element>> elements(flattenedTree.size());
    std::transform(
        flattenedTree.begin(), flattenedTree.end(),
        elements.begin(), [](const auto &e) { return std::weak_ptr(e); });

    _elements.reserve(_elements.size() + elements.size());

    for (
        auto ownerNextSibling = ctx->getOwner()->getNextSibling();
        ownerNextSibling != nullptr;
        ownerNextSibling = ownerNextSibling->getNextSibling()) {

        if (!ownerNextSibling->hasItsOwnStackingContext()) {
            auto it = std::find_if(
                _elements.begin(),
                _elements.end(),
                [ownerNextSibling](const std::weak_ptr<ui::element::Element>& e) { return e.lock() == ownerNextSibling; });
            _elements.insert(std::next(it), elements.begin(), elements.end());
            processed = true;
            break;
        }
    }

    if (!processed) { // insert at the end
        _elements.insert(_elements.end(), elements.begin(), elements.end());
    }*/
}

bool StackingContext::IsRequiredFor(std::shared_ptr<const ui::element::Element> element) {
    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return element->isRoot() || style.opacity < 1.0f ||
           (style.transform.has_value() && !style.transform->isSetToDefault()) ||
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
            e->setStackingContext(parentCtx);
        }

        for (auto child : e->getChildren())
            queue.push({child, e->getStackingContext()});
    }

    return rootCtx;
}

} // namespace rendering
} // namespace ui