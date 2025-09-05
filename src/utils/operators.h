#pragma once 

#include <compare>
#include <raylib.h>

bool operator!=(const Color& lhs, const Color& rhs);

bool operator==(const Color& colorA, const Color& colorB);

std::strong_ordering operator<=>(const Color& colorA, const Color& colorB);

bool operator==(const Vector2& vecA, const Vector2& vecB);

bool operator!=(const Vector2& vecA, const Vector2& vecB);