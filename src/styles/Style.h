#ifndef __STYLE_H__
#define __STYLE_H__

#include <optional>
#include "./Flex.h"
#include "./Size.h"
#include "./Overflow.h"
#include "./Display.h"
#include "./Spacing.h"
#include "./PositionType.h"

namespace style
{
    struct Style
    {
        std::optional<Size> size;
        std::optional<Spacing> spacing;
        std::optional<Flex> flex;
        std::optional<PositionType> position;
        std::optional<Display> display;
        std::optional<Overflow> overflow;
    };
}

#endif