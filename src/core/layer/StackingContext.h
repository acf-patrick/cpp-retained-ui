#pragma once

namespace layer {

struct StackingContext {
    int zIndex;

    bool operator<=>(const StackingContext&) const = default;
};

} // namespace layer