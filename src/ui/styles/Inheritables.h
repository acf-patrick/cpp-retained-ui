#pragma once

#include <raylib.h>

#include "./MaybeInherited.h"

namespace ui {
namespace style {

struct Inheritables {
    MaybeInherited<Color> color;

    void updateInheritedFields(const Inheritables& source);

    bool operator<=>(const Inheritables &) const = default;
};

} // namespace style
} // namespace ui