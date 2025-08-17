#include "./functions.h"

namespace utils {

void drawRectangle(const Rectangle& rect,
                   float top,
                   float bottom,
                   float left,
                   float right,
                   const Color& topColor,
                   const Color& bottomColor,
                   const Color& leftColor,
                   const Color& rightColor) {
  auto divBy2 = [](float num) { return num <= 1.0 ? num : (num / 2); };

  if (top > 0.0 && topColor.a > 0)
    DrawLineEx(Vector2{.x = rect.x, .y = rect.y + divBy2(top)},
               Vector2{.x = rect.x + rect.width, .y = rect.y + divBy2(top)},
               top, topColor);

  if (left > 0.0 && leftColor.a > 0)
    DrawLineEx(Vector2{.x = rect.x + divBy2(left), .y = rect.y},
               Vector2{.x = rect.x + divBy2(left), .y = rect.y + rect.height},
               left, leftColor);

  if (bottom > 0.0 && bottomColor.a > 0)
    DrawLineEx(Vector2{.x = rect.x, .y = rect.y + rect.height - divBy2(bottom)},
               Vector2{.x = rect.x + rect.width,
                       .y = rect.y + rect.height - divBy2(bottom)},
               bottom, bottomColor);

  if (right > 0.0 && rightColor.a > 0)
    DrawLineEx(Vector2{.x = rect.x + rect.width - divBy2(right), .y = rect.y},
               Vector2{.x = rect.x + rect.width - divBy2(right),
                       .y = rect.y + rect.height},
               right, rightColor);
}

float clampRatio(float ratio) {
  return ratio < 0 ? 0.0 : ratio > 1 ? 1.0 : ratio;
}
}  // namespace utils