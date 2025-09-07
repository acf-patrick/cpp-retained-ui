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
    _cleanRenderTexture = true;
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

    auto owner = _owner.lock();
    const auto transform = ctx.transform;

    Rectangle dest{
        .x = 0,
        .y = 0,
        .width = (float)_renderTexture.texture.width,
        .height = (float)_renderTexture.texture.height};

    float rotation = 0.0;

    if (ctx.transform.has_value()) {
        const auto transform = *ctx.transform;
        if (auto scale = transform.scale) {
            dest = {
                .x = 0,
                .y = 0,
                .width = scale->x * _renderTexture.texture.width, // take in count scale
                .height = scale->y * _renderTexture.texture.height};
        }

        if (auto tRotation = transform.rotation) {
            if (auto deg = std::get_if<utils::AngleDegree>(&tRotation->angle)) {
                rotation = deg->value;
            } else {
                rotation = 180 / PI * std::get<utils::AngleRadian>(tRotation->angle).value;
            }
        }

        if (auto translation = transform.translation) {
            if (auto value = std::get_if<utils::Value<float>>(&translation->x)) {
                dest.x = value->value;
            } else {
                auto ratio = std::get<utils::Ratio>(translation->x);
                dest.x = ratio.ratio * dest.width;
            }

            if (auto value = std::get_if<utils::Value<float>>(&translation->y)) {
                dest.y = value->value;
            } else {
                auto ratio = std::get<utils::Ratio>(translation->y);
                dest.y = ratio.ratio * dest.height;
            }
        }
    }

    Vector2 origin;
    const auto transformOrigin = owner->getStyle().transformOrigin;
    if (std::holds_alternative<ui::style::TransformOriginCenter>(transformOrigin)) {
        origin.x = 0.5 * dest.width;
        origin.y = 0.5 * dest.height;
    } else {
        auto originPosition = std::get<ui::style::TransformOriginPosition>(transformOrigin);

        if (auto value = std::get_if<utils::Value<int>>(&originPosition.x)) {
            origin.x = value->value;
        } else {
            auto ratio = std::get<utils::Ratio>(originPosition.x);
            origin.x = ratio.ratio * dest.width;
        }

        if (auto value = std::get_if<utils::Value<int>>(&originPosition.y)) {
            origin.y = value->value;
        } else {
            auto ratio = std::get<utils::Ratio>(originPosition.y);
            origin.y = ratio.ratio * dest.width;
        }
    }

    dest.x += origin.x;
    dest.y += origin.y;

    DrawTexturePro(_renderTexture.texture,
                   Rectangle{
                       .x = 0,
                       .y = 0,
                       .width = (float)_renderTexture.texture.width,
                       .height = -(float)_renderTexture.texture.height},
                   dest, origin, rotation,
                   Color{
                       .r = 255,
                       .g = 255,
                       .b = 255,
                       .a = (unsigned char)alpha});

    _cleanRenderTexture = false;
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

void Layer::sortChildrenByZIndex() {
    std::sort(
        _children.begin(),
        _children.end(),
        [](const std::shared_ptr<Layer> &ctxA, const std::shared_ptr<Layer> &ctxB) {
            return ctxA->getContext().zIndex >= ctxB->getContext().zIndex;
        });
}

void Layer::appendChild(std::shared_ptr<Layer> child) {
    if (!child)
        return;

    _children.push_back(child);
    sortChildrenByZIndex();
    child->setParent(shared_from_this());
}

void Layer::removeChild(std::shared_ptr<Layer> child) {
    if (child) {
        _children.erase(std::remove(_children.begin(), _children.end(), child));
        sortChildrenByZIndex();
    }
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

void Layer::repositionInParent() {
    if (auto parent = _parent.lock())
        parent->sortChildrenByZIndex();
}

std::shared_ptr<ui::element::Element> Layer::getOwner() const {
    return _owner.lock();
}

bool Layer::isClean() const {
    return _cleanRenderTexture;
}

void Layer::clearRenderTarget() {
    BeginTextureMode(_renderTexture);
    ClearBackground(BLANK);
    EndTextureMode();

    _cleanRenderTexture = true;
}

bool Layer::IsRequiredFor(std::shared_ptr<const ui::element::Element> element) {
    if (!StackingContext::IsRequiredFor(element))
        return false;

    if (element == nullptr)
        return false;

    const auto style = element->getStyle();
    return element->isRoot() ||
           style.opacity < 1.0f ||
           style.transform.has_value() ||
           std::holds_alternative<ui::style::IsolationIsolate>(style.isolation);
    /*
        or hasFilter
        or hasBackdropEffect
    */


    /*
    TODO : also check transform default value
    */
}

std::shared_ptr<Layer> Layer::BuildTree(std::shared_ptr<StackingContext> rootCtx) {
    // [currentCtx, parentLayer]
    std::queue<std::pair<std::shared_ptr<ui::rendering::StackingContext>, std::shared_ptr<ui::rendering::Layer>>> queue;
    std::shared_ptr<ui::rendering::Layer> rootLayer;

    queue.push({rootCtx, nullptr});
    while (!queue.empty()) {
        auto [ctx, parentLayer] = queue.front();
        queue.pop();
        auto ctxOwner = ctx->getOwner();

        if (ui::rendering::Layer::IsRequiredFor(ctxOwner)) {
            auto layer = std::make_shared<ui::rendering::Layer>(ctxOwner);
            ctx->setLayer(layer);
            if (parentLayer)
                parentLayer->appendChild(layer);

            if (ctx == rootCtx)
                rootLayer = layer;
        } else {
            ctx->setLayer(parentLayer);
        }

        for (auto child : ctx->getChildren())
            queue.push({child, ctx->getLayer()});
    }

    return rootLayer;
}

} // namespace rendering
} // namespace ui