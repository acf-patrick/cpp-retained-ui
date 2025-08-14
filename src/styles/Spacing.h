#ifndef __SPACING_H__
#define __SPACING_H__

#include <optional>

namespace style
{
    struct Spacing
    {
        std::optional<int> margin; // margin for all four directions
        std::optional<int> marginLeft;
        std::optional<int> marginRight;
        std::optional<int> marginTop;
        std::optional<int> marginBottom;

        std::optional<int> padding; // padding for all four directions
        std::optional<int> paddingLeft;
        std::optional<int> paddingRight;
        std::optional<int> paddingTop;
        std::optional<int> paddingBottom;

        std::optional<int> border; // border for all four directions
        std::optional<int> borderLeft;
        std::optional<int> borderRight;
        std::optional<int> borderTop;
        std::optional<int> borderBottom;

        std::optional<float> marginRatio; // ratio of parent's margin
        std::optional<float> marginRatioLeft;
        std::optional<float> marginRatioRight;
        std::optional<float> marginRatioTop;
        std::optional<float> marginRatioBottom;
    };
}

#endif