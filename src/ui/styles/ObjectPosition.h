#pragma once

#include <utility>
#include <variant>

#include "./Edge.h"

namespace ui {
namespace style {

struct ObjectPositionCenter {
    bool operator<=>(const ObjectPositionCenter &) const = default;
};

struct ObjectPositionPosition {
    float x, y;

    bool operator<=>(const ObjectPositionPosition &) const = default;
};

struct ObjectPositionEdge {
    Edge x, y;

    bool operator<=>(const ObjectPositionEdge &) const = default;
};

struct ObjectPositionRatio {
    float x, y;

    bool operator<=>(const ObjectPositionRatio &) const = default;
};

using ObjectPosition = std::variant<ObjectPositionCenter, ObjectPositionPosition, ObjectPositionRatio, ObjectPositionEdge>;

} // namespace style
} // namespace ui