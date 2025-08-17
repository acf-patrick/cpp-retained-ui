#include "./operators.h"

bool operator!=(const Color& lhs, const Color& rhs) {
  return !(lhs == rhs);
}

bool operator==(const Color& colorA, const Color& colorB) {
  return colorA.r == colorB.r && colorA.g == colorB.g && colorA.b == colorB.b &&
         colorA.a == colorB.a;
}

std::strong_ordering operator<=>(const Color& lhs, const Color& rhs) {
  if (auto cmp = lhs.r <=> rhs.r; cmp != 0)
    return cmp;
  if (auto cmp = lhs.g <=> rhs.g; cmp != 0)
    return cmp;
  if (auto cmp = lhs.b <=> rhs.b; cmp != 0)
    return cmp;
  return lhs.a <=> rhs.a;
}