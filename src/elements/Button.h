#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "./Element.h"
#include <iostream>
#include <yoga/YGNodeLayout.h>

namespace element
{
    class Button : public Element
    {
        sf::RectangleShape _rect;

    public:
        Button()
        {
            _rect.setFillColor(sf::Color(255, 0, 255));
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

            for (auto &child : _children)
            {
                child->render(window);
            }
        }
    };
}

#endif