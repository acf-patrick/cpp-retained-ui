#include "./Root.h"
#include "../defaults.h"
#include <iostream>
#include <queue>
#include <unordered_set>

namespace ui {
namespace element {

Root::Root(const Vector2 &windowSize) : Element("Root") {
    YGNodeFree(_yogaNode);
    _config = YGConfigNew();
    YGConfigSetUseWebDefaults(_config, true);
    _yogaNode = YGNodeNewWithConfig(_config);
    

    updateLayout(ui::defaults::rootLayout(windowSize));
}

Root::~Root() {
    YGConfigFree(_config);
}

void Root::finalize() {
    updateStyle(ui::defaults::rootStyles(_preferredTheme));
    propagatePreferredTheme();
    propagateStyles();
    _finalized = true;
}

void Root::propagatePreferredTheme() {
    std::queue<Element *> queue;
    queue.push(this);
    std::unordered_set<unsigned int> visitedIds;

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (visitedIds.contains(node->getId()))
            continue;

        if (node != this)
            node->setPreferredTheme(_preferredTheme);

        visitedIds.emplace(node->getId());
        for (auto &child : node->getChildren())
            queue.push(child.get());
    }
}

void Root::onLayoutDirtyFlagTriggered() {
    _dirtyLayout = true;
}

void Root::onPreferredThemeChanged(ui::style::Theme theme) {
    updateStyle(ui::defaults::rootStyles(theme));
    propagatePreferredTheme();
}

void Root::calculateLayout() {
    YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);

    std::queue<Element *> queue;
    queue.push(this);
    std::unordered_set<unsigned int> visitedIds;

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (visitedIds.contains(node->getId()))
            continue;

        node->updateAbsolutePosition();

        visitedIds.emplace(node->getId());
        for (auto &child : node->getChildren())
            queue.push(child.get());
    }

    _dirtyLayout = false;
}

void Root::propagateStyles() {
    std::queue<Element *> queue;
    queue.push(this);
    std::unordered_set<unsigned int> visitedIds;

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (visitedIds.contains(node->getId()))
            continue;

        if (node->_dirtyCachedInheritableProps) {
            if (node != this) {
                node->updateCachedInheritablePropsFrom(node->getParent());
                node->_dirtyCachedInheritableProps = false;
            }

            visitedIds.emplace(node->getId());
            for (auto &child : node->getChildren())
                queue.push(child.get());
        }
    }

    _dirtyCachedInheritableProps = false;
}

void Root::update() {
    if (_dirtyLayout)
        calculateLayout();

    if (_dirtyCachedInheritableProps)
        propagateStyles();
}

void Root::render() {
    if (!_finalized)
        throw std::logic_error("[Root] Rendering non-finalized root element");

    if (_dirtyLayout)
        throw std::logic_error("[Root] Rendering dirty layout");

    if (_dirtyCachedInheritableProps)
        throw std::logic_error("[Root] Rendering with inherited styles not recalculated");

    std::queue<Element *> queue;
    queue.push(this);
    std::unordered_set<unsigned int> visitedIds;

    ClearBackground(_style.backgroundColor.value_or(BLACK));

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (visitedIds.contains(node->getId()) || node->isNotDisplayed())
            continue;

        if (node != this)
            node->render();

        visitedIds.emplace(node->getId());
        for (auto &child : node->getChildren())
            queue.push(child.get());
    }
}

} // namespace element
} // namespace ui