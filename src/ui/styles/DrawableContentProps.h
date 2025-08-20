#pragma once

#include "./ObjectFit.h"
#include "./ObjectPosition.h"

namespace ui {
namespace style {

struct DrawableContentProps {
    ObjectPosition objectPosition;
    ObjectFit objectFit;
};

} // namespace style
} // namespace ui