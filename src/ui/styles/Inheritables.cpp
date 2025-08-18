#include "./Inheritables.h"

namespace ui {
namespace style {

void Inheritables::updateInheritedFields(const Inheritables &source) {
    if (color.isInherited() && !source.color.isInherited())
        color = source.color.unwrap();
}

} // namespace style
} // namespace ui