#pragma once

#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>
#include <SFML/Graphics.hpp>

#include <vector>
#include <memory>
#include <algorithm>
#include <variant>
#include <unordered_map>

#include "../styles/Style.h"
#include "../utils/functions.h"

namespace element
{
    class Element
    {
        style::Style _style;
        static unsigned int nextId;

    protected:
        YGNodeRef _yogaNode;
        unsigned int _id;
        std::vector<std::shared_ptr<Element>> _children;

        void updateOverflow(style::Overflow overflow);
        void updateDisplay(style::Display display);
        void updatePosition(style::PositionType position);
        void updateFlex(const style::Flex &flex);
        void updateSpacing(const style::Spacing &spacing);
        void updateSize(const style::Size &size);

    public:
        Element();
        virtual ~Element();

        virtual void render(sf::RenderWindow &window) = 0;

        void appendChild(std::shared_ptr<Element> child);

        void removeChild(std::shared_ptr<Element> child);

        void removeAllChildren();

        void updateStyle(const style::Style &style);

        unsigned int getId() const;

        style::Style getStyle() const;

        std::vector<std::shared_ptr<Element>> getChildren();
    };
}
