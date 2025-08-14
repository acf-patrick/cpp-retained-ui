#pragma once

#include "./Element.h"

namespace element
{
    class Root : public Element
    {
        YGConfigRef _config;

    public:
        Root(const sf::Vector2u &windowSize);
        ~Root();
        void render(sf::RenderWindow &window) override;
    };
}