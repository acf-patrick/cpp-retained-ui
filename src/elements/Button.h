#pragma once

#include "./Element.h"
#include <yoga/YGNodeLayout.h>

namespace element
{
    class Button : public Element
    {
        sf::RectangleShape _rect;

    public:
        Button(const sf::Color& color)
        {
            _rect.setFillColor(color);
        }

        void render(sf::RenderWindow &window) override
        {
            float left = YGNodeLayoutGetLeft(_yogaNode);
            float top = YGNodeLayoutGetTop(_yogaNode);
            float width = YGNodeLayoutGetWidth(_yogaNode);
            float height = YGNodeLayoutGetHeight(_yogaNode);

            _rect.setPosition(sf::Vector2f(left, top));
            _rect.setSize({width, height});
            window.draw(_rect);
        }
    };
}

