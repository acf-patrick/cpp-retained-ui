#include "./StackingContext.h"
#include "../elements/Element.h"
#include "./Layer.h"

#include <algorithm>
#include <format>
#include <queue>
#include <set>
#include <stack>

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

Vector2 StackingContext::render(ScissorStack& scissorStack, const Vector2& parentOffset) {
    auto layer = getLayer();
    if (!layer) {
        TraceLog(LOG_ERROR, "[StackingContext] %d does not have a layer", _id);
        return;
    }

    auto self = shared_from_this();
    auto owner = getOwner();
    if (!owner) {
        TraceLog(LOG_ERROR, "[StackingContext] %d does not have an owner element", _id);
        return;
    }

    if (!layer->isClean())
        layer->clearRenderTarget();

    Vector2 offset = parentOffset;
    if (hasItsOwnLayer()) // begin a new drawing origin
        offset.x = offset.y = 0.0;

    // TODO : we may want to update layer's position (cache)

    auto useLayerGuard = layer->use();
    if (!owner->isNotDisplayed()) {
        std::stack<std::pair<std::shared_ptr<ui::element::Element>, Vector2>> stack;
        stack.push({ owner, offset });

        while (!stack.empty()) {
            auto [e, offset] = stack.top();
            stack.pop();

            // TODO : use scissor stack

            if (!e->isNotDisplayed() && e->belongsTo(self)) {
                e->render(offset);
                const auto rect = e->getBoundingRect();
                const Vector2 newOffset { offset.x + rect.x, offset.y + rect.y };

                for (auto child : e->getChildren())
                    stack.push({ child, newOffset });
            }
        }
    }

    const auto rect = owner->getBoundingRect();
    return Vector2 { offset.x + rect.x, offset.y + rect.y };
}

void StackingContext::renderTree() {
    ScissorStack scissorStack;
    std::stack<std::pair<std::shared_ptr<StackingContext>, Vector2>> stack;
    stack.push({ shared_from_this(), Vector2 { 0.0, 0.0 } });

    while (!stack.empty()) {
        auto [ctx, parentOffset] = stack.top();
        stack.pop();

        const auto newOffset = ctx->render(scissorStack, parentOffset);
        for (auto child : ctx->getChildren())
            stack.push({ child, newOffset });
    }
}

std::vector<std::shared_ptr<ui::element::Element>> StackingContext::getElements() const {
    auto owner = _owner.lock();
    if (!owner) {
        const auto errorMessage = std::format("[StackingContext] %d does not have an owner element", _id);
        TraceLog(LOG_ERROR, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }

    std::queue<std::shared_ptr<ui::element::Element>> queue;
    std::vector<std::shared_ptr<ui::element::Element>> elements;
    queue.push(owner);

    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();

        elements.push_back(e);
        for (auto child : e->getChildren())
            elements.push_back(child);
    }

    return elements;
}

void StackingContext::skipChild(std::shared_ptr<StackingContext> child) {
    if (!child)
        return;

    if (std::find(_children.begin(), _children.end(), child) == _children.end()) {
        const auto errorMessage =
            std::format("[StackingContext] Can not skip a non-direct children of this context");
        TraceLog(LOG_ERROR, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }

    auto self = shared_from_this();

    for (auto grandChild : child->getChildren()) {
        grandChild->setParent(self);
        appendChild(grandChild);
    }
    sortChildrenByZIndex();
    takeOwnershipOfElements(child);

    removeChild(child);
}

void StackingContext::takeOwnershipOfElements(std::shared_ptr<StackingContext> ctx) {
    if (ctx == nullptr)
        return;

    auto self = shared_from_this();
    std::queue<std::shared_ptr<ui::element::Element>> queue;
    queue.push(ctx->getOwner());

    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();

        if (e->getStackingContext() == ctx) {
            e->setStackingContext(self);
            for (auto child : e->getChildren())
                queue.push(child);
        }
    }
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