#include "./functions.h"

namespace utils
{
    float clampRatio(float ratio)
    {
        return ratio < 0 ? 0.0 : ratio > 1 ? 1.0
                                           : ratio;
    }
}