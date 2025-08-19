#pragma once

#include <raylib.h>
#include <string>
#include <vector>

#include "./MaybeInherited.h"

namespace ui {
namespace style {

struct Inheritables {
    MaybeInherited<Color> color;
    MaybeInherited<unsigned int> fontSize; // in pixels
    MaybeInherited<std::vector<std::string>> fontFamily;

    void updateInheritedFields(const Inheritables &source, const Inheritables &newProps);

    bool operator<=>(const Inheritables &) const = default;
};

} // namespace style
} // namespace ui