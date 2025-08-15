#include "./Root.h"

#include <queue>
#include <unordered_set>

namespace ui {
namespace element {

Root::Root(const Vector2& windowSize) {
  YGNodeFree(_yogaNode);
  _config = YGConfigNew();
  YGConfigSetUseWebDefaults(_config, true);
  _yogaNode = YGNodeNewWithConfig(_config);

  style::Style rootStyle;
  auto& size = rootStyle.size.emplace();
  size.width = windowSize.x;
  size.height = windowSize.y;

  auto& flex = rootStyle.flex.emplace();
  flex.gap.emplace(10);
  flex.justifyContent.emplace(ui::style::JustifyContent::SpaceBetween);

  updateStyle(rootStyle);
}

Root::~Root() {
  YGConfigFree(_config);
}

void Root::render() {
  YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);

  std::queue<Element*> queue;
  queue.push(this);
  std::unordered_set<unsigned int> visitedIds;

  while (!queue.empty()) {
    auto node = queue.front();
    queue.pop();

    if (visitedIds.count(node->getId()) > 0)
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