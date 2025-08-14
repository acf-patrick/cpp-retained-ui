#include "./Root.h"

#include <unordered_set>
#include <queue>

namespace element
{
    Root::Root(const sf::Vector2u &windowSize)
    {
        YGNodeFree(_yogaNode);
        _config = YGConfigNew();
        YGConfigSetUseWebDefaults(_config, true);
        _yogaNode = YGNodeNewWithConfig(_config);

        style::Style rootStyle;
        auto& size = rootStyle.size.emplace();
        size.width = windowSize.x;
        size.height = windowSize.y;

        updateStyle(rootStyle);
    }

    Root::~Root()
    {
        YGConfigFree(_config);
    }

    void Root::render(sf::RenderWindow &window)
    {
        YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);

        std::queue<Element*> queue;
        queue.push(this);
        std::unordered_set<unsigned int> visitedIds;

        while (!queue.empty())
        {
            auto node = queue.front();
            queue.pop();

            if (visitedIds.count(node->getId()) > 0)
                continue;

            if (node != this)
                node->render(window);

            visitedIds.emplace(node->getId());
            for (auto &child : node->getChildren())
                queue.push(child.get());
        }
    }
}