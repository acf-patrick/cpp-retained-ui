#pragma once

namespace ui{
namespace layer {

struct LayerContext {
    int zIndex;
    float opacity;

    bool operator<=>(const LayerContext&) const = default;
};

} // namespace layer
}