#include "./Root.h"
#include "../defaults.h"

#include "../rendering.h"

#include <queue>

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
    TraceLog(LOG_INFO, "[Root] Finalizing element tree");

    updateStyle(ui::defaults::rootStyles(_preferredTheme));
    propagatePreferredTheme();
    calculateLayout();
    propagateStyles();
    _finalized = true;
}

void Root::propagatePreferredTheme() {
    std::queue<std::shared_ptr<Element>> queue;
    auto self = shared_from_this();
    queue.push(self);

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (node != self)
            node->setPreferredTheme(_preferredTheme);

        for (auto &child : node->getChildren())
            queue.push(child);
    }
}

void Root::onLayoutDirtyFlagTriggered() {
    _dirtyLayout = true;
}

void Root::onDirtyCachedInheritableStylesTriggered() {
    propagateStyles();
    _dirtyCachedInheritableProps = false; // indicate we have cleared that flag
}

void Root::onPreferredThemeChanged(ui::style::Theme theme) {
    updateStyle(ui::defaults::rootStyles(theme));
    propagatePreferredTheme();
}

void Root::calculateLayout() {
    YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);

    std::queue<std::shared_ptr<Element>> queue;
    auto self = shared_from_this();
    queue.push(self);

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        node->updateAbsolutePosition();
        for (auto &child : node->getChildren())
            queue.push(child);
    }

    _dirtyLayout = false;
}

void Root::propagateStyles() {
    std::queue<std::shared_ptr<Element>> queue;
    auto self = shared_from_this();
    queue.push(self);

    while (!queue.empty()) {
        auto node = queue.front();
        queue.pop();

        if (node->_dirtyCachedInheritableProps) {
            if (node != self) {
                node->updateCachedInheritablePropsFrom(node->getParent());
                node->_dirtyCachedInheritableProps = false;
            }

            for (auto &child : node->getChildren())
                queue.push(child);
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
    std::string errorMessage;
    if (!_finalized)
        errorMessage = "[Root] Rendering non-finalized root element\n";

    if (_dirtyLayout)
        errorMessage = "[Root] Rendering dirty layout\n";

    if (_dirtyCachedInheritableProps)
        errorMessage = "[Root] Rendering with inherited styles not recalculated\n";

    if (!errorMessage.empty()) {
        TraceLog(LOG_FATAL, errorMessage.c_str());
        throw std::logic_error(errorMessage);
    }

    Element::render();
}

void Root::onWindowResized(int newWidth, int newHeight) {
    auto layout = getLayout();
    auto size = layout.size.value_or(ui::style::Size{});
    size.width = utils::Value(newWidth);
    size.height = utils::Value(newHeight);
    layout.size = size;
    updateLayout(layout);
}

} // namespace element
} // namespace ui