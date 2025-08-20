#include "./Inheritables.h"

namespace ui {
namespace style {

template <typename T>
void applyInherited(MaybeInherited<T> &output, const MaybeInherited<T> &source, const MaybeInherited<T> &newProp) {
    if (source.isInherited() && !newProp.isInherited())
        output = newProp.unwrap();
}

void Inheritables::updateInheritedFields(const Inheritables &source, const Inheritables &newProps) {
    applyInherited(color, source.color, newProps.color);
    applyInherited(fontSize, source.fontSize, newProps.fontSize);
    applyInherited(fontFamily, source.fontFamily, newProps.fontFamily);
    applyInherited(letterSpacing, source.letterSpacing, newProps.letterSpacing);
}

} // namespace style
} // namespace ui