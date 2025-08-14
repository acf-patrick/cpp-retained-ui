#ifndef __ELEMENT_H__
#define __ELEMENT_H__

#include <yoga/YGNode.h>
#include <yoga/YGNodeStyle.h>
#include <SFML/Graphics.hpp>

#include <vector>
#include <memory>
#include <algorithm>
#include "../styles/Style.h"

namespace element
{
    class Element
    {
    protected:
        YGNodeRef _yogaNode;
        std::vector<std::shared_ptr<Element>> _children;

    public:
        Element()
        {
            _yogaNode = YGNodeNew();
        }

        virtual ~Element()
        {
            YGNodeFree(_yogaNode);
        }

        virtual void render(sf::RenderWindow &window) = 0;

        void appendChild(std::shared_ptr<Element> child)
        {
            if (!child)
                return;

            _children.push_back(child);
            YGNodeInsertChild(_yogaNode, child->_yogaNode, _children.size() - 1);
        }

        void removeChild(std::shared_ptr<Element> child)
        {
            auto it = std::find(_children.begin(), _children.end(), child);
            if (it != _children.end())
            {
                YGNodeRemoveChild(_yogaNode, child->_yogaNode);
                _children.erase(it);
            }
        }

        void removeAllChildren()
        {
            YGNodeRemoveAllChildren(_yogaNode);
            _children.clear();
        }

        void updateStyle(const style::Style &style)
        {
            /* POC using flex-direction, width/height and margin */

            if (style.flex)
            {
                if (auto dir = style.flex->flexDirection)
                {
                    switch (*dir)
                    {
                    case style::FlexDirection::Row:
                        YGNodeStyleSetFlexDirection(_yogaNode, YGFlexDirectionRow);
                        break;
                    case style::FlexDirection::Column:
                        YGNodeStyleSetFlexDirection(_yogaNode, YGFlexDirectionColumn);
                        break;
                    case style::FlexDirection::RowReverse:
                        YGNodeStyleSetFlexDirection(_yogaNode, YGFlexDirectionRowReverse);
                        break;
                    case style::FlexDirection::ColumnReverse:
                        YGNodeStyleSetFlexDirection(_yogaNode, YGFlexDirectionColumnReverse);
                        break;
                    }
                }
            }

            if (auto size = style.size)
            {
                if (auto width = size->width)
                {
                    YGNodeStyleSetWidth(_yogaNode, *width);
                }
                if (auto height = size->height)
                {
                    YGNodeStyleSetHeight(_yogaNode, *height);
                }
            }

            if (auto spacing = style.spacing)
            {
                if (auto margin = spacing->margin)
                {
                    YGNodeStyleSetMargin(_yogaNode, YGEdgeAll, *margin);
                }
            }
        }
    };
}

#endif