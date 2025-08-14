#ifndef __SIZE_H__
#define __SIZE_H__

#include <optional>

namespace style
{
    struct Size
    {
        Size() = default;
        std::optional<int> width;  // set explicit width
        std::optional<int> height; // set explicit height
        std::optional<int> minWidth;
        std::optional<int> minHeight;
        std::optional<int> maxWidth;
        std::optional<int> maxHeight;
        std::optional<float> widthRatio;  // ratio of parent width
        std::optional<float> heightRatio; // ratio of parent height
        std::optional<float> aspectRatio; // if only width or height is set, the other dimension is computed from aspectRatio.
    };
}

#endif