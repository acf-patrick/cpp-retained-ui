#include "./Root.h"
#include "../defaults.h"

#include <queue>
#include <unordered_set>

namespace ui {
namespace element {

Root::Root(const Vector2& windowSize) {
  YGNodeFree(_yogaNode);
  _config = YGConfigNew();
  YGConfigSetUseWebDefaults(_config, true);
  _yogaNode = YGNodeNewWithConfig(_config);

  updateLayout(ui::defaults::rootLayout(windowSize));
}

Root::~Root() {
  YGConfigFree(_config);
}

bool Root::shouldRecalculateLayout() const {
  return _dirty;
}

void Root::finalize() {
  updateStyle(ui::defaults::rootStyles(_preferredTheme));
  propagatePreferredTheme();
  _ok = true;
}

void Root::propagatePreferredTheme() {
  std::queue<Element*> queue;
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
    for (auto& child : node->getChildren())
      queue.push(child.get());
  }
}

void Root::onDirtyFlagTriggered() {
  _dirty = true;
}

void Root::onPreferredThemeChanged(ui::style::Theme theme) {
  updateStyle(ui::defaults::rootStyles(theme));
  propagatePreferredTheme();
}

void Root::calculateLayout() {
  YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);

  std::queue<Element*> queue;
  queue.push(this);
  std::unordered_set<unsigned int> visitedIds;

  while (!queue.empty()) {
    auto node = queue.front();
    queue.pop();

    if (visitedIds.contains(node->getId()))
      continue;

    node->updateAbsolutePosition();

    visitedIds.emplace(node->getId());
    for (auto& child : node->getChildren())
      queue.push(child.get());
  }

  _dirty = false;
}

void Root::render() {
  if (!_ok)
    throw std::logic_error("Rendering non-finalized root element");

  std::queue<Element*> queue;
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
    for (auto& child : node->getChildren())
      queue.push(child.get());
  }
}

}  // namespace element
}  // namespace ui