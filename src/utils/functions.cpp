#include "./functions.h"

#include <algorithm>
#include <cmath>

namespace utils {

bool isMultipleOf2PI(double angle) {
    double _2PI = 2.0 * PI;
    double r = std::fmod(angle, _2PI);
    return std::fabs(r) < 1e-9 || std::fabs(r - _2PI) < 1e-9;
}

Rectangle getBoundsOfTransformedRect(
    const Rectangle &rect,
    const Vector2 &origin,
    float rotation,
    const Vector2 &scale,
    const Vector2 &translation) {
    const auto tl = transformPoint({rect.x, rect.y}, origin, rotation, scale, translation);
    const auto tr = transformPoint({rect.x + rect.width, rect.y}, origin, rotation, scale, translation);
    const auto bl = transformPoint({rect.x, rect.y + rect.height}, origin, rotation, scale, translation);
    const auto br = transformPoint({rect.x + rect.width, rect.y + rect.height}, origin, rotation, scale, translation);

    Rectangle bb{
        .x = std::min({tl.x, tr.x, bl.x, br.x}),
        .y = std::min({tl.y, tr.y, bl.y, br.y}),
        .width = 0,
        .height = 0};

    bb.width = std::max({tl.x, tr.x, bl.x, br.x}) - bb.x;
    bb.height = std::max({tl.y, tr.y, bl.y, br.y}) - bb.y;

    return bb;
}

Vector2 transformPoint(const Vector2 &point,
                       const Vector2 &origin,
                       float rotation,
                       const Vector2 &scale,
                       const Vector2 &translation) {
    // Scale
    Vector2 transformed = {
        .x = (point.x - origin.x) * scale.x + origin.x,
        .y = (point.y - origin.y) * scale.y + origin.y};

    // Rotate
    float cosR = std::cos(rotation);
    float sinR = std::sin(rotation);
    float x = cosR * (transformed.x - origin.x) - sinR * (transformed.y - origin.y) + origin.x;
    float y = sinR * (transformed.x - origin.x) + cosR * (transformed.y - origin.y) + origin.y;

    // Translate
    transformed.x += translation.x;
    transformed.y += translation.y;

    return transformed;
}

void drawRectangle(const Rectangle &rect,
                   float top,
                   float bottom,
                   float left,
                   float right,
                   const Color &topColor,
                   const Color &bottomColor,
                   const Color &leftColor,
                   const Color &rightColor) {
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
    return ratio < 0 ? 0.0 : ratio > 1 ? 1.0
                                       : ratio;
}
} // namespace utils