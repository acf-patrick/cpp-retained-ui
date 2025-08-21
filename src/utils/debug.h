#pragma once

#include <format>
#include <raylib.h>
#include <string>

/*
Enable printing with std::println : Specialization classes are defined here
*/

template <>
struct std::formatter<Color> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Color &color, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "Color [{}, {}, {}, {}]", color.r, color.g, color.b, color.a);
    }
};

template <>
struct std::formatter<Rectangle> {
    constexpr auto parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    auto format(const Rectangle &rect, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "Rectangle [{}, {}, {}, {}]", rect.x, rect.y, rect.width, rect.height);
    }
};