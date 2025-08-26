#pragma once

#include <elements/Element.h>

#include <raylib.h>

#include <map>
#include <memory>
#include <vector>

#include "./StackingContext.h"

// forward declaration
namespace ui {
namespace element {
class Root;
}
} // namespace ui

namespace layer {

class LayerManager {
  public:
    class Layer {
        // RAII struct for rendering on texture
        struct UseTexture {
            RenderTexture2D renderTexture;

            UseTexture(const RenderTexture2D &texture);
            ~UseTexture();
        };

        RenderTexture2D _renderTexture;
        std::vector<std::shared_ptr<ui::element::Element>> _elements;

      public:
        Layer();
        ~Layer();

        void clear();
        void render();
        void add(std::shared_ptr<ui::element::Element> element);
        void remove(std::shared_ptr<ui::element::Element> element);
        std::shared_ptr<ui::element::Element> hitTest(const Vector2 &point) const;
    };

  private:
    static LayerManager *instance;

    std::map<StackingContext, std::shared_ptr<Layer>> _layers;

    static void DeleteInstance();

    LayerManager() = default;
    ~LayerManager();

  public:
    static LayerManager &Get();

    void renderLayers();
    std::shared_ptr<ui::element::Element> hitTestLayers(const Vector2 &point) const;

    friend class ui::element::Root;
};

} // namespace layer