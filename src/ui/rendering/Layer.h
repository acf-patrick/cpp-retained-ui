#pragma once

#include <raylib.h>

#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "../styles/Transform.h"

namespace ui {

namespace style { // forward declaration
class Style;
}

namespace element {
class Element; // forward declaration
}

namespace rendering {

class Layer : public std::enable_shared_from_this<Layer> {
  public:
    using LayerId = unsigned int;

    class UseLayerGuard {
        std::optional<RenderTexture2D> _texture;

      public:
        UseLayerGuard() = default;
        UseLayerGuard(RenderTexture2D texture) : _texture(texture) { BeginTextureMode(texture); }
        ~UseLayerGuard() {
            if (_texture)
                EndTextureMode();
        }
    };

    using UseLayerGuardRef = std::shared_ptr<UseLayerGuard>;

    struct Context {
        int zIndex;    // > 0
        float opacity; // in ]0; 1[
        std::optional<ui::style::Transform> transform;

        Context() = default;
        Context(const ui::style::Style &style);
    };

  private:
    static LayerId nextId;

    LayerId _id;
    std::vector<std::shared_ptr<Layer>> _children;
    std::weak_ptr<Layer> _parent;
    RenderTexture2D _renderTexture;
    bool _clean;

    // bool _used; // tells if redering are currently performed on this layer's texture
    std::weak_ptr<ui::element::Element> _owner;

  public:
    Layer(std::shared_ptr<ui::element::Element> owner);
    ~Layer();

    void render();

    UseLayerGuardRef use();

    void compositeChildren();

    void setOwner(std::shared_ptr<ui::element::Element> owner);
    std::shared_ptr<ui::element::Element> getOwner() const;

    LayerId getId() const;
    Context getContext() const;
    std::vector<std::shared_ptr<Layer>> getChildren() const;

    void setParent(std::shared_ptr<Layer> parent);

    std::shared_ptr<Layer> getParent() const;
    void removeChild(std::shared_ptr<Layer> child);

    bool clean() const;

    // set clean flag to false
    void markAsDirty();

    void clearRenderTarget();

    void appendChild(std::shared_ptr<Layer> child);

    template <typename... Layers>
    void appendChildren(std::shared_ptr<Layer> parent, std::shared_ptr<Layers> &&...layers) {
        (appendChild(layers), ...);
    }

    static bool IsRequiredFor(std::shared_ptr<ui::element::Element> element);
};

} // namespace rendering
} // namespace ui