#pragma once

#include <raylib.h>

namespace utils {

float clampRatio(float ratio);

Rectangle getBoundsOfTransformedRect(const Rectangle &rect,
                                       const Vector2 &origin,
                                       float rotation,
                                       const Vector2 &scale,
                                       const Vector2 &translation);

Vector2 transformPoint(const Vector2 &point,
                       const Vector2 &origin,
                       float rotation,
                       const Vector2 &scale,
                       const Vector2 &translation);

// Draw rectangle by drawing four edges individualy
void drawRectangle(const Rectangle &rect,
                   float top,
                   float bottom,
                   float left,
                   float right,
                   const Color &topColor,
                   const Color &bottomColor,
                   const Color &leftColor,
                   const Color &rightColor);

} // namespace utils