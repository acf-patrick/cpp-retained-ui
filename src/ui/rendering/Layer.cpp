#include "./Layer.h"

#include <algorithm>
#include <queue>
#include <set>

#include "../elements/Element.h"

ui::rendering::Layer::LayerId ui::rendering::Layer::nextId = 0;

namespace ui {
namespace rendering {

Layer::Layer(const Layer::Context &ctx, std::shared_ptr<Layer> parent)
    : _parent(parent), _context(ctx) {
    _id = nextId++;
    _renderTexture = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    if (_renderTexture.id == 0)
        throw std::runtime_error("[Layer] Unable to create render texture.");
}

Layer::~Layer() {
    UnloadRenderTexture(_renderTexture);
}

void Layer::render() {
    BeginTextureMode(_renderTexture);
    {
        std::queue<std::shared_ptr<element::Element>> queue;
        std::set<unsigned int> visitedIds;

        for (auto element : _elements)
            queue.push(element);

        while (!queue.empty()) {
            auto element = queue.front();
            queue.pop();

            if (visitedIds.contains(element->getId()))
                continue;

            element->render();
            auto children = element->getChildren();
            for (auto child : children) {
                if (auto layer = child->getLayer())
                    if (layer->_id == _id)
                        queue.push(child);
            }

            visitedIds.emplace(element->getId());
        }
    }
    EndTextureMode();

    DrawTexture(_renderTexture.texture, 0, 0, WHITE);
    // DrawTextureRec(_renderTexture, )

    for (auto child : _children)
        child->render();
}

void Layer::setContext(const Layer::Context &ctx) {
    _context = ctx;
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
        _elements.erase(std::remove(_elements.begin(), _elements.end(), element));
}

std::shared_ptr<ui::element::Element> Layer::hitTest(const Vector2 &point) const {
    throw std::logic_error("Layer::hitTest not implemented yet");

    for (auto child : _children) {

        if (auto element = child->hitTest(point)) {
        }
    }

    for (auto it = _elements.rbegin(); it != _elements.rend(); ++it) {
        auto element = *it;
        if (element->contains(point))
            return element;
    }
    return nullptr;
}

void Layer::setParent(std::shared_ptr<Layer> parent) {
    _parent = parent;
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