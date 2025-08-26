#include "./Layer.h"

#include <algorithm>

namespace layer {

LayerManager::~LayerManager() {
    instance = nullptr;
}

LayerManager &LayerManager::Get() {
    if (!instance)
        instance = new LayerManager();
    return *instance;
}

void LayerManager::renderLayers() {
    for (auto &[_, layer] : _layers)
        layer->render();
}

/* Layer */

LayerManager::Layer::Layer() {
    _renderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    if (_renderTexture.id == 0)
        throw std::runtime_error("[Layer] Unable to create render texture.");
}

LayerManager::Layer::~Layer() {
    UnloadRenderTexture(_renderTexture);
}

void LayerManager::Layer::render() {
    UseTexture useTexture(_renderTexture);
    for (auto element : _elements)
        element->render();
}

void LayerManager::Layer::add(std::shared_ptr<ui::element::Element> element) {
    _elements.push_back(element);
}

void LayerManager::Layer::remove(std::shared_ptr<ui::element::Element> element) {
    _elements.erase(std::remove(_elements.begin(), _elements.end(), element));
}

std::shared_ptr<ui::element::Element> LayerManager::Layer::hitTest(const Vector2 &point) const {
    for (auto it = _elements.rbegin(); it != _elements.rend(); ++it) {
        auto element = *it;
        if (element->contains(point))
            return element;
    }
    return nullptr;
}

} // namespace layer