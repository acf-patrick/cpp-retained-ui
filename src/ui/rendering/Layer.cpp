#include "./Layer.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

#include "../elements/Element.h"
#include "../styles/Style.h"
#include "./StackingContext.h"

ui::rendering::Layer::LayerId ui::rendering::Layer::nextId = 0;

namespace ui {
namespace rendering {

Layer::Context::Context(const ui::style::Style &style) {
    opacity = style.opacity;
}

Layer::Layer(std::shared_ptr<ui::element::Element> owner)
    : _owner(owner) {
    _id = nextId++;
    _renderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    if (_renderTexture.id == 0) {
        const std::string errorMessage("[Layer] Unable to create render texture.");
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::runtime_error(errorMessage);
    }
}

Layer::~Layer() {
    UnloadRenderTexture(_renderTexture);
}

void Layer::render() {
    throw std::logic_error("Layer::render not implemented yet");
}

Layer::Context Layer::getContext() const {
    if (auto owner = _owner.lock())
        return Context(owner->getStyle());

    TraceLog(LOG_ERROR, "[Layer] %d has null owner element", _id);
    throw std::runtime_error("[Layer] Null owner");
}

std::shared_ptr<Layer> Layer::getParent() const {
    return _parent.lock();
}

Layer::LayerId Layer::getId() const {
    return _id;
}

std::vector<std::shared_ptr<Layer>> Layer::getChildren() const {
    return _children;
}

void Layer::appendChild(std::shared_ptr<Layer> child) {
    if (child)
        _children.push_back(child);
}

void Layer::removeChild(std::shared_ptr<Layer> child) {
    if (child)
        _children.erase(std::remove(_children.begin(), _children.end(), child));
}

void Layer::addElement(std::shared_ptr<ui::element::Element> element) {
    if (element)
        _elements.push_back(element);
}

void Layer::removeElement(std::shared_ptr<ui::element::Element> element) {
    if (element)
        _elements.erase(
            std::remove_if(_elements.begin(), _elements.end(),
                           [=](std::weak_ptr<ui::element::Element> e) { return e.lock() == element; }));
}

void Layer::removeAllElements() {
    _elements.clear();
}

void Layer::compositeChildren() {
    //   _owner
}

Layer::UseLayerGuardRef Layer::use() {
    return std::make_shared<UseLayerGuard>(_renderTexture);
}

std::shared_ptr<ui::element::Element> Layer::hitTest(const Vector2 &point) const {
    throw std::logic_error("Layer::hitTest not implemented yet");
}

void Layer::setParent(std::shared_ptr<Layer> parent) {
    _parent = parent;
}

void Layer::setOwner(std::shared_ptr<ui::element::Element> owner) {
    if (!owner) {
        const std::string errorMessage("[Layer] nullptr provided as owner");
        TraceLog(LOG_ERROR, errorMessage.c_str());
    } else
        _owner = owner;
}

std::shared_ptr<ui::element::Element> Layer::getOwner() const {
    return _owner.lock();
}

bool Layer::IsRequiredFor(std::shared_ptr<ui::element::Element> element) {
    if (!StackingContext::IsRequiredFor(element))
        return false;
    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return style.opacity < 1.0f;
    /*
        or hasTransform
        or hasFilter
        or hasBackdropEffect
    */
}

std::shared_ptr<Layer> Layer::AppendChild(std::shared_ptr<Layer> parent, std::shared_ptr<Layer> child) {
    if (parent == nullptr || child == nullptr)
        return parent;

    parent->appendChild(child);
    child->setParent(parent);

    return parent;
}

} // namespace rendering
} // namespace ui