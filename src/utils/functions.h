#pragma once

#include <raylib.h>

namespace utils {
float clampRatio(float ratio);

void drawRectangle(const Rectangle& rect,
                   float top,
                   float bottom,
                   float left,
                   float right,
                   const Color& topColor,
                   const Color& bottomColor,
                   const Color& leftColor,
                   const Color& rightColor);
}  // namespace utils