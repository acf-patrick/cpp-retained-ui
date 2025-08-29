#pragma once

#include <raylib.h>

#include <map>
#include <memory>
#include <vector>

// forward declaration
namespace ui {
namespace element {
class Element;
}

namespace rendering {

class Layer {
  public:
    using LayerId = unsigned int;

    struct Context {
        int zIndex;
        float opacity; // in ]0; 1[
    };

  private:
    static LayerId nextId;

    LayerId _id;
    std::vector<std::shared_ptr<Layer>> _children;
    std::weak_ptr<Layer> _parent;
    RenderTexture2D _renderTexture;
    Context _context;
    bool _used; // tells if redering are currently performed on this layer's texture
    std::vector<std::shared_ptr<ui::element::Element>> _elements;

  public:
    Layer(const Context &ctx, std::shared_ptr<Layer> parent = nullptr);
    ~Layer();

    void clear();
    void render();

    LayerId getId() const;

    void setContext(const Context &ctx);

    std::shared_ptr<Layer> getParent() const;
    void addChild(std::shared_ptr<Layer> child);
    void removeChild(std::shared_ptr<Layer> child);

    void addElement(std::shared_ptr<ui::element::Element> element);
    void removeElement(std::shared_ptr<ui::element::Element> element);

    std::shared_ptr<ui::element::Element> hitTest(const Vector2 &point) const;
};

/*
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
*/
} // namespace rendering
} // namespace ui