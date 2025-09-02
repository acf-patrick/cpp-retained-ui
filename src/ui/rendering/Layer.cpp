#include "./Layer.h"

#include <algorithm>
#include <queue>

#include "../elements/Element.h"
#include "../styles/Style.h"
#include "./StackingContext.h"

ui::rendering::Layer::LayerId ui::rendering::Layer::nextId = 0;

namespace ui {
namespace rendering {

Layer::Context::Context(const ui::style::Style &style) {
    opacity = style.opacity;
    zIndex = style.zIndex;
    transform = style.transform;
}

Layer::Layer(std::shared_ptr<ui::element::Element> owner)
    : _owner(owner) {
    _id = nextId++;
    _clean = true;
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
    const auto ctx = getContext();
    int alpha = ctx.opacity * 255; // get correct alpha
    if (alpha < 0)
        alpha = 0;
    if (alpha > 255)
        alpha = 255;

    // TODO : apply transform here

    DrawTextureRec(_renderTexture.texture,
                   Rectangle{
                       .x = 0,
                       .y = 0,
                       .width = (float)_renderTexture.texture.width,
                       .height = -(float)_renderTexture.texture.height},
                   Vector2{0, 0},
                   Color{.r = 255, .g = 255, .b = 255, .a = (unsigned char)alpha});
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

void Layer::compositeChildren() {
    //   _owner
}

Layer::UseLayerGuardRef Layer::use() {
    return std::make_shared<UseLayerGuard>(_renderTexture);
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

bool Layer::clean() const {
    return _clean;
}

void Layer::clearRenderTarget() {
    BeginTextureMode(_renderTexture);
    ClearBackground(BLANK);
    EndTextureMode();

    _clean = true;
}

void Layer::markAsDirty() {
    _clean = false;
}

bool Layer::IsRequiredFor(std::shared_ptr<ui::element::Element> element) {
    if (!StackingContext::IsRequiredFor(element))
        return false;

    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return element->isRoot() || style.opacity < 1.0f || style.transform.has_value();
    /*
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