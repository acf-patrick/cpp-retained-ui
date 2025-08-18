#include "./Inheritables.h"

namespace ui {
namespace style {

void Inheritables::updateInheritedFields(const Inheritables &source, const Inheritables &newProps) {
    if (source.color.isInherited() && !newProps.color.isInherited())
        color = newProps.color.unwrap();
}

} // namespace style
} // namespace ui