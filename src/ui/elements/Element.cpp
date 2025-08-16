#include "./Element.h"

#include <yoga/YGNodeLayout.h>

#include <algorithm>
#include <unordered_map>

namespace ui {
namespace element {

unsigned int Element::nextId = 0;

std::shared_ptr<Element> Element::AppendChild(std::shared_ptr<Element> parent,
                          std::shared_ptr<Element> child) {
  parent->appendChild(child);
  child->_parent = parent;

  return parent;
}

Element::Element() {
  _id = nextId++;
  _yogaNode = YGNodeNew();
  _absolutePosition = {.x = 0.0, .y = 0.0};
}

Element::~Element() {
  YGNodeFree(_yogaNode);
}

void Element::appendChild(std::shared_ptr<Element> child) {
  if (!child)
    return;

  _children.push_back(child);
  YGNodeInsertChild(_yogaNode, child->_yogaNode, _children.size() - 1);
}

void Element::removeChild(std::shared_ptr<Element> child) {
  auto it = std::find(_children.begin(), _children.end(), child);
  if (it != _children.end()) {
    YGNodeRemoveChild(_yogaNode, child->_yogaNode);
    _children.erase(it);
  }
}

void Element::removeAllChildren() {
  YGNodeRemoveAllChildren(_yogaNode);
  _children.clear();
}

Rectangle Element::getBoundingRect() const {
  Rectangle bb = {.x = _absolutePosition.x,
                  .y = _absolutePosition.y,
                  .width = YGNodeLayoutGetWidth(_yogaNode),
                  .height = YGNodeLayoutGetHeight(_yogaNode)};
  return bb;
}

std::shared_ptr<Element> Element::getParent() {
  if (auto parent = _parent.lock())
    return parent;
  return nullptr;
}

std::vector<std::shared_ptr<Element>> Element::getChildren() {
  return _children;
}

std::vector<std::shared_ptr<Element>> Element::getSiblings() {
  std::vector<std::shared_ptr<Element>> siblings;

  if (auto parent = _parent.lock()) {
    for (auto sibling : parent->_children)
      if (sibling->_id != _id)
        siblings.emplace_back(sibling);
  }

  return siblings;
}

unsigned int Element::getId() const {
  return _id;
}

style::Style Element::getStyle() const {
  return _style;
}

void Element::updateAbsolutePosition() {
  if (auto parent = _parent.lock()) {
    _absolutePosition.x =
        YGNodeLayoutGetLeft(_yogaNode) + parent->_absolutePosition.x;
    _absolutePosition.y =
        YGNodeLayoutGetTop(_yogaNode) + parent->_absolutePosition.y;
  }
}

void Element::updateStyle(const style::Style& style) {
  if (auto flex = style.flex)
    updateFlex(*flex);

  if (auto size = style.size)
    updateSize(*size);

  if (auto spacing = style.spacing)
    updateSpacing(*spacing);

  if (auto position = style.position)
    updatePosition(*position);

  if (auto display = style.display)
    updateDisplay(*display);

  if (auto overflow = style.overflow)
    updateOverflow(*overflow);

  _style = style;
}

void Element::updateOverflow(style::Overflow overflow) {
  std::unordered_map<style::Overflow, YGOverflow> overflows = {
      {style::Overflow::Hidden, YGOverflowHidden},
      {style::Overflow::Scroll, YGOverflowScroll},
      {style::Overflow::Visible, YGOverflowVisible}};

  YGNodeStyleSetOverflow(_yogaNode, overflows[overflow]);
}

void Element::updateDisplay(style::Display display) {
  std::unordered_map<style::Display, YGDisplay> displays = {
      {style::Display::Contents, YGDisplayContents},
      {style::Display::None, YGDisplayNone},
      {style::Display::Flex, YGDisplayFlex}};

  YGNodeStyleSetDisplay(_yogaNode, displays[display]);
}

void Element::updatePosition(style::PositionType position) {
  std::unordered_map<style::PositionType, YGPositionType> positions = {
      {style::PositionType::Absolute, YGPositionTypeAbsolute},
      {style::PositionType::Relative, YGPositionTypeRelative},
      {style::PositionType::Static, YGPositionTypeStatic},
  };

  YGNodeStyleSetPositionType(_yogaNode, positions[position]);
}

void Element::updateFlex(const style::Flex& flex) {
  std::unordered_map<style::FlexDirection, YGFlexDirection> flexDirections = {
      {style::FlexDirection::Column, YGFlexDirectionColumn},
      {style::FlexDirection::ColumnReverse, YGFlexDirectionColumnReverse},
      {style::FlexDirection::Row, YGFlexDirectionRow},
      {style::FlexDirection::RowReverse, YGFlexDirectionRowReverse},
  };

  std::unordered_map<style::JustifyContent, YGJustify> justifyContents = {
      {style::JustifyContent::FlexStart, YGJustifyFlexStart},
      {style::JustifyContent::Center, YGJustifyCenter},
      {style::JustifyContent::FlexEnd, YGJustifyFlexEnd},
      {style::JustifyContent::SpaceBetween, YGJustifySpaceBetween},
      {style::JustifyContent::SpaceAround, YGJustifySpaceAround},
      {style::JustifyContent::SpaceEvenly, YGJustifySpaceEvenly},
  };

  std::unordered_map<style::Alignment, YGAlign> alignments = {
      {style::Alignment::FlexStart, YGAlignFlexStart},
      {style::Alignment::Center, YGAlignCenter},
      {style::Alignment::FlexEnd, YGAlignFlexEnd},
      {style::Alignment::Stretch, YGAlignStretch},
      {style::Alignment::Baseline, YGAlignBaseline},
      {style::Alignment::Auto, YGAlignAuto},
  };

  if (auto flexDirection = flex.flexDirection)
    YGNodeStyleSetFlexDirection(_yogaNode, flexDirections[*flexDirection]);

  if (auto justifyContent = flex.justifyContent)
    YGNodeStyleSetJustifyContent(_yogaNode, justifyContents[*justifyContent]);

  if (auto alignItems = flex.alignItems)
    YGNodeStyleSetAlignItems(_yogaNode, alignments[*alignItems]);

  if (auto alignSelf = flex.alignSelf)
    YGNodeStyleSetAlignSelf(_yogaNode, alignments[*alignSelf]);

  if (auto flexGrow = flex.flexGrow)
    YGNodeStyleSetFlexGrow(_yogaNode, *flexGrow);

  if (auto flexShrink = flex.flexShrink)
    YGNodeStyleSetFlexShrink(_yogaNode, *flexShrink);

  if (auto flexBasis = flex.flexBasis) {
    if (std::get_if<style::FlexBasisAuto>(&(*flexBasis)))
      YGNodeStyleSetFlexBasisAuto(_yogaNode);

    if (auto percent = std::get_if<style::FlexBasisPercent>(&(*flexBasis)))
      YGNodeStyleSetFlexBasisPercent(_yogaNode, percent->value);

    if (auto value = std::get_if<style::FlexBasisValue>(&(*flexBasis)))
      YGNodeStyleSetFlexBasis(_yogaNode, value->value);
  }

  if (auto gap = flex.gap)
    YGNodeStyleSetGap(_yogaNode, YGGutterAll, *gap);

  if (auto rowGap = flex.rowGap)
    YGNodeStyleSetGap(_yogaNode, YGGutterRow, *rowGap);

  if (auto columnGap = flex.columnGap)
    YGNodeStyleSetGap(_yogaNode, YGGutterColumn, *columnGap);

  if (auto gapRatio = flex.gapRatio)
    YGNodeStyleSetGapPercent(_yogaNode, YGGutterAll,
                             100 * utils::clampRatio(*gapRatio));

  if (auto rowGapRatio = flex.rowGapRatio)
    YGNodeStyleSetGapPercent(_yogaNode, YGGutterRow,
                             100 * utils::clampRatio(*rowGapRatio));

  if (auto columnGapRatio = flex.columnGapRatio)
    YGNodeStyleSetGapPercent(_yogaNode, YGGutterColumn,
                             100 * utils::clampRatio(*columnGapRatio));
}

void Element::updateSpacing(const style::Spacing& spacing) {
  // margin

  if (auto margin = spacing.margin)
    YGNodeStyleSetMargin(_yogaNode, YGEdgeAll, *margin);

  if (auto l = spacing.marginLeft)
    YGNodeStyleSetMargin(_yogaNode, YGEdgeLeft, *l);

  if (auto r = spacing.marginRight)
    YGNodeStyleSetMargin(_yogaNode, YGEdgeRight, *r);

  if (auto t = spacing.marginTop)
    YGNodeStyleSetMargin(_yogaNode, YGEdgeTop, *t);

  if (auto b = spacing.marginBottom)
    YGNodeStyleSetMargin(_yogaNode, YGEdgeBottom, *b);

  // padding

  if (auto padding = spacing.padding)
    YGNodeStyleSetPadding(_yogaNode, YGEdgeAll, *padding);

  if (auto l = spacing.paddingLeft)
    YGNodeStyleSetPadding(_yogaNode, YGEdgeLeft, *l);

  if (auto r = spacing.paddingRight)
    YGNodeStyleSetPadding(_yogaNode, YGEdgeRight, *r);

  if (auto t = spacing.paddingTop)
    YGNodeStyleSetPadding(_yogaNode, YGEdgeTop, *t);

  if (auto b = spacing.paddingBottom)
    YGNodeStyleSetPadding(_yogaNode, YGEdgeBottom, *b);

  // border

  if (auto border = spacing.border)
    YGNodeStyleSetBorder(_yogaNode, YGEdgeAll, *border);

  if (auto l = spacing.borderLeft)
    YGNodeStyleSetBorder(_yogaNode, YGEdgeLeft, *l);

  if (auto r = spacing.borderRight)
    YGNodeStyleSetBorder(_yogaNode, YGEdgeRight, *r);

  if (auto t = spacing.borderTop)
    YGNodeStyleSetBorder(_yogaNode, YGEdgeTop, *t);

  if (auto b = spacing.borderBottom)
    YGNodeStyleSetBorder(_yogaNode, YGEdgeBottom, *b);

  // margin-ratio

  if (auto ratio = spacing.marginRatio)
    YGNodeStyleSetMarginPercent(_yogaNode, YGEdgeAll,
                                100 * utils::clampRatio(*ratio));

  if (auto l = spacing.marginRatioLeft)
    YGNodeStyleSetMarginPercent(_yogaNode, YGEdgeLeft,
                                100 * utils::clampRatio(*l));

  if (auto r = spacing.marginRatioRight)
    YGNodeStyleSetMarginPercent(_yogaNode, YGEdgeRight,
                                100 * utils::clampRatio(*r));

  if (auto t = spacing.marginRatioTop)
    YGNodeStyleSetMarginPercent(_yogaNode, YGEdgeTop,
                                100 * utils::clampRatio(*t));

  if (auto b = spacing.marginRatioBottom)
    YGNodeStyleSetMarginPercent(_yogaNode, YGEdgeBottom,
                                100 * utils::clampRatio(*b));

  // padding-ratio

  if (auto ratio = spacing.paddingRatio)
    YGNodeStyleSetPaddingPercent(_yogaNode, YGEdgeAll,
                                 100 * utils::clampRatio(*ratio));

  if (auto l = spacing.paddingRatioLeft)
    YGNodeStyleSetPaddingPercent(_yogaNode, YGEdgeLeft,
                                 100 * utils::clampRatio(*l));

  if (auto r = spacing.paddingRatioRight)
    YGNodeStyleSetPaddingPercent(_yogaNode, YGEdgeRight,
                                 100 * utils::clampRatio(*r));

  if (auto t = spacing.paddingRatioTop)
    YGNodeStyleSetPaddingPercent(_yogaNode, YGEdgeTop,
                                 100 * utils::clampRatio(*t));

  if (auto b = spacing.paddingRatioBottom)
    YGNodeStyleSetPaddingPercent(_yogaNode, YGEdgeBottom,
                                 100 * utils::clampRatio(*b));
}

void Element::updateSize(const style::Size& size) {
  if (auto width = size.width)
    YGNodeStyleSetWidth(_yogaNode, *width);

  if (auto height = size.height)
    YGNodeStyleSetHeight(_yogaNode, *height);

  if (auto mw = size.minWidth)
    YGNodeStyleSetMinWidth(_yogaNode, *mw);

  if (auto mh = size.minHeight)
    YGNodeStyleSetMinHeight(_yogaNode, *mh);

  if (auto mw = size.maxWidth)
    YGNodeStyleSetMinWidth(_yogaNode, *mw);

  if (auto mh = size.maxHeight)
    YGNodeStyleSetMinHeight(_yogaNode, *mh);

  if (auto widthRatio = size.widthRatio)
    YGNodeStyleSetWidthPercent(_yogaNode, 100 * utils::clampRatio(*widthRatio));

  if (auto heightRatio = size.heightRatio)
    YGNodeStyleSetWidthPercent(_yogaNode,
                               100 * utils::clampRatio(*heightRatio));

  if (auto aspectRatio = size.aspectRatio)
    YGNodeStyleSetWidthPercent(_yogaNode,
                               100 * utils::clampRatio(*aspectRatio));
}

}  // namespace element
}  // namespace ui