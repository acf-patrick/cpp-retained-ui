#include "./Element.h"

#include "../../utils/functions.h"
#include "../../utils/operators.h"
#include "../defaults.h"
#include "../rendering/Layer.h"
#include "../rendering/StackingContext.h"

#include <yoga/YGNodeLayout.h>

#include <algorithm>
#include <format>
#include <queue>
#include <unordered_map>

namespace ui {
namespace element {

Element::ElementId Element::nextId = 0;

Element::Element(const std::string &name)
    : _preferredTheme(ui::style::Theme::Dark), _name(name), _dirtyCachedInheritableProps(true) {
    _id = nextId++;
    _absolutePosition = {.x = 0.0, .y = 0.0};

    _yogaNode = YGNodeNew();
    updateStyle(ui::defaults::elementStyles(_preferredTheme));
    updateLayout(ui::defaults::elementLayout());
}

Element::~Element() {
    YGNodeFree(_yogaNode);
}

bool Element::isRoot() const {
    return _parent.lock() == nullptr;
}

bool Element::hasItsOwnStackingContext() const {
    if (auto ctx = _stackingContext.lock())
        return ctx->getOwner().get() == this;

    return false;
}

bool Element::needsItsOwnStackingContext() const {
    auto self = shared_from_this();
    return ui::rendering::StackingContext::IsRequiredFor(self);
}

bool Element::isNotDisplayed() const {
    if (auto display = _layout.display) {
        return *display == ui::style::Display::None;
    }

    return _style.opacity == 0;
}

void Element::onChildRemoved(std::shared_ptr<Element>) { /* Do nothing */ }

void Element::onChildAppended(std::shared_ptr<Element>) { /* Do nothing */ }

void Element::onLayoutDirtyFlagTriggered() { /* Do nothing */ }

void Element::onDirtyCachedInheritableStylesTriggered() { /* Do nothing */ }

void Element::onPreferredThemeChanged(ui::style::Theme theme) {
    updateStyle(ui::defaults::elementStyles(theme));
}

void Element::markInheritableStylesAsDirty() {
    // walk up the tree
    for (auto parent = _parent.lock(); parent; parent = parent->getParent()) {
        if (!parent->_dirtyCachedInheritableProps) {
            parent->_dirtyCachedInheritableProps = true;
            parent->onDirtyCachedInheritableStylesTriggered();
        }
    }

    // notify children
    std::queue<Element *> queue;
    queue.push(this);

    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();

        if (!e->_dirtyCachedInheritableProps) {
            e->_dirtyCachedInheritableProps = true;
            e->onDirtyCachedInheritableStylesTriggered();

            for (auto child : e->_children)
                queue.push(child.get());
        }
    }
}

void Element::markLayoutAsDirty() {
    onLayoutDirtyFlagTriggered();

    // notify parent for layout recomputation
    for (auto parent = _parent.lock(); parent; parent = parent->getParent())
        parent->onLayoutDirtyFlagTriggered();
}

Element &Element::appendChild(std::shared_ptr<Element> child) {
    auto &self = *this;
    if (!child)
        return self;

    for (auto parent = getParent(); parent; parent = parent->getParent()) {
        if (parent == child) {
            const std::string errorMessage("[Element] Provided element is an ancestor of this element.");
            TraceLog(LOG_FATAL, errorMessage.c_str());
            throw std::logic_error(errorMessage);
        }
    }

    _children.push_back(child);
    child->setParent(shared_from_this());
    YGNodeInsertChild(_yogaNode, child->_yogaNode, _children.size() - 1);
    onChildAppended(child);

    return self;
}

void Element::removeChild(std::shared_ptr<Element> child) {
    auto it = std::find(_children.begin(), _children.end(), child);
    if (it != _children.end()) {
        YGNodeRemoveChild(_yogaNode, child->_yogaNode);
        (*it)->_parent.reset();
        _children.erase(it);

        onChildRemoved(child);
    }
}

void Element::removeAllChildren() {
    YGNodeRemoveAllChildren(_yogaNode);
    _children.clear();
}

Vector2 Element::getPosition() const {
    const auto rect = getBoundingRect();
    return Vector2{rect.x, rect.y};
}

Vector2 Element::getSize() const {
    const auto rect = getBoundingRect();
    return Vector2{rect.width, rect.height};
}

bool Element::contains(const Vector2 &point) const {
    const auto rect = getBoundingRect();
    return CheckCollisionPointRec(point, rect);
}

Rectangle Element::getFinalBoundingRect() const {
    const auto bb = getBoundingRect();
    if (!_style.transform.has_value())
        return bb;

    Vector2 origin;
    if (std::holds_alternative<style::TransformOriginCenter>(_style.transformOrigin)) {
        origin.x = bb.width / 2;
        origin.y = bb.height / 2;
    } else {
        auto& originPosition = std::get<style::TransformOriginPosition>(_style.transformOrigin);

        if (auto x = std::get_if<utils::Value<int>>(&originPosition.x)) {
            origin.x = x->value;
        } else {
            origin.x = bb.width * std::get<utils::Ratio>(originPosition.x).ratio;
        }

        if (auto y = std::get_if<utils::Value<int>>(&originPosition.y)) {
            origin.y = y->value;
        } else {
            origin.y = bb.height * std::get<utils::Ratio>(originPosition.y).ratio;
        }
    }

    float rotationAngle = 0.0;
    if (auto rotation = _style.transform->rotation) {
        if (auto deg = std::get_if<utils::AngleDegree>(&rotation->angle)) {
            rotationAngle = DEG2RAD * deg->value;
        } else {
            rotationAngle = std::get<utils::AngleRadian>(rotation->angle).value;
        }
    }

    Vector2 scale = {1.0, 1.0};
    if (auto tScale = _style.transform->scale) {
        scale = *tScale;
    }

    Vector2 translation = {0.0, 0.0};
    if (auto tTranslation = _style.transform->translation) {
        if (auto x = std::get_if<utils::Value<float>>(&tTranslation->x)) {
            translation.x = x->value;
        } else {
            translation.x = bb.width * std::get<utils::Ratio>(tTranslation->x).ratio;
        }

        if (auto y = std::get_if<utils::Value<float>>(&tTranslation->y)) {
            translation.y = y->value;
        } else {
            translation.y = bb.height * std::get<utils::Ratio>(tTranslation->y).ratio;
        }
    }

    return utils::getBoundsOfTransformedRect(bb, origin, rotationAngle, scale, translation);
}

Rectangle Element::getBoundingRect() const {
    Rectangle bb = {.x = _absolutePosition.x,
                    .y = _absolutePosition.y,
                    .width = YGNodeLayoutGetWidth(_yogaNode),
                    .height = YGNodeLayoutGetHeight(_yogaNode)};
    return bb;
}

std::shared_ptr<Element> Element::getParent() const {
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

Element::ElementId Element::getId() const {
    return _id;
}

style::Style Element::getStyle() const {
    return _style;
}

style::Layout Element::getLayout() const {
    return _layout;
}

void Element::updateAbsolutePosition() {
    if (auto parent = _parent.lock()) {
        _absolutePosition.x =
            YGNodeLayoutGetLeft(_yogaNode) + parent->_absolutePosition.x;
        _absolutePosition.y =
            YGNodeLayoutGetTop(_yogaNode) + parent->_absolutePosition.y;
    }
}

void Element::updateStyle(const style::Style &style) {
    if (style.inheritables != _style.inheritables) {
        _cachedInheritableProps = style.inheritables;
        markInheritableStylesAsDirty();
    }

    auto tmp = _style;
    _style = style;
    checkForStackingContextAndLayerUpdate(tmp);
}

void Element::checkForStackingContextAndLayerUpdate(const style::Style &oldStyle) {
    auto ctx = _stackingContext.lock();
    if (!ctx)
        return;
    /* TODO : runtime_error should be thrown only after UI elements setup
     ```{
       const auto errorMessage = std::format("[Element] {} does not own and is not owned by any StackingContext", _id);
       TraceLog(LOG_ERROR, errorMessage.c_str());
       throw std::runtime_error(errorMessage);
    }``` */

    if (!hasItsOwnStackingContext()) {
        if (needsItsOwnStackingContext()) {
            auto self = shared_from_this();
            auto newCtx = ui::rendering::StackingContext::BuildTree(self);

            if (newCtx->needsItsOwnLayer()) {
                auto newLayer = ui::rendering::Layer::BuildTree(newCtx);
                newCtx->setLayer(newLayer);
                ctx->getLayer()->appendChild(newLayer);
            } else
                newCtx->setLayer(ctx->getLayer());

            ctx->appendChild(newCtx);
            _stackingContext = newCtx;
        }
    } else {
        if (!needsItsOwnStackingContext()) {
            if (ctx->hasItsOwnLayer())
                disposeOwnedLayer();

            if (auto parentCtx = ctx->getParent()) {
                parentCtx->skipChild(ctx);
                _stackingContext = parentCtx;
            } else {
                throw std::runtime_error("[Element] Element seems to be root although does not need its own stacking context");
            }
        } else { // need and has its own stacking context
            if (oldStyle.zIndex != _style.zIndex) {
                ctx->repositionInParent();
            }

            if (!ctx->hasItsOwnLayer() && ctx->needsItsOwnLayer()) {
                auto newLayer = ui::rendering::Layer::BuildTree(ctx);
                auto parentLayer = ctx->getLayer();
                ctx->setLayer(newLayer);
                parentLayer->appendChild(newLayer);
            } else if (ctx->hasItsOwnLayer() && !ctx->needsItsOwnLayer()) {
                disposeOwnedLayer();
            } else if (ctx->needsItsOwnLayer() && ctx->hasItsOwnLayer()) {
                if (oldStyle.zIndex != _style.zIndex)
                    ctx->getLayer()->repositionInParent();
            }
        }
    }
}

void Element::disposeOwnedLayer() {
    if (!hasItsOwnStackingContext())
        return;

    auto ctx = _stackingContext.lock();
    if (!ctx->hasItsOwnLayer())
        return;

    auto layer = ctx->getLayer();
    if (auto parentLayer = layer->getParent()) {
        parentLayer->removeChild(layer);
        ctx->setLayer(parentLayer);
    }
}

std::vector<std::shared_ptr<Element>> Element::flatten() const {
    std::vector<std::shared_ptr<Element>> tree;
    std::queue<std::shared_ptr<Element>> queue;

    auto self = std::const_pointer_cast<Element>(shared_from_this());
    queue.push(self);

    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();

        tree.push_back(e);
        for (auto child : e->_children)
            queue.push(child);
    }

    return tree;
}

std::shared_ptr<Element> Element::getPreviousSibling() const {
    auto self = shared_from_this();

    if (auto parent = _parent.lock()) {
        auto it = std::find(
            parent->_children.begin(),
            parent->_children.end(),
            self);

        if (it == parent->_children.begin())
            return nullptr;
        else {
            auto prev = std::prev(it);
            return *prev;
        }
    }

    return nullptr;
}

std::shared_ptr<Element> Element::getNextSibling() const {
    auto self = shared_from_this();

    if (auto parent = _parent.lock()) {
        auto it = std::find(
                      parent->_children.begin(),
                      parent->_children.end(),
                      self) +
                  1;

        return it == parent->_children.end() ? nullptr : (*it);
    }

    return nullptr;
}

void Element::updateLayout(const style::Layout &layout) {
    if (_layout == layout)
        return;

    if (auto flex = layout.flex)
        updateFlex(*flex);

    if (auto size = layout.size)
        updateSize(*size);

    if (auto spacing = layout.spacing)
        updateSpacing(*spacing);

    if (auto position = layout.position)
        updatePosition(*position);

    if (auto position = layout.positionType)
        updatePositionType(*position);

    if (auto display = layout.display)
        updateDisplay(*display);

    if (auto overflow = layout.overflow)
        updateOverflow(*overflow);

    if (auto boxSizing = layout.boxSizing)
        updateBoxSizing(*boxSizing);

    markLayoutAsDirty();
    _layout = layout;
}

void Element::updateBoxSizing(style::BoxSizing boxSizing) {
    std::unordered_map<style::BoxSizing, YGBoxSizing> sizings = {
        {style::BoxSizing::BorderBox, YGBoxSizingBorderBox},
        {style::BoxSizing::ContentBox, YGBoxSizingContentBox}};

    YGNodeStyleSetBoxSizing(_yogaNode, sizings[boxSizing]);
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

void Element::updatePosition(const style::Position &position) {
    std::unordered_map<YGEdge, std::optional<utils::ValueRatioAuto<float>>>
        positions = {{YGEdgeLeft, position.left},
                     {YGEdgeRight, position.right},
                     {YGEdgeTop, position.top},
                     {YGEdgeBottom, position.bottom}};

    for (auto &[edge, optPosition] : positions) {
        if (auto position = optPosition) {
            if (auto value = std::get_if<utils::Value<float>>(&*position))
                YGNodeStyleSetPosition(_yogaNode, edge, value->value);

            if (auto ratio = std::get_if<utils::Ratio>(&*position))
                YGNodeStyleSetPosition(_yogaNode, edge, ratio->ratio);

            if (std::holds_alternative<utils::Auto>(*position))
                YGNodeStyleSetPositionAuto(_yogaNode, edge);
        }
    }
}

void Element::updatePositionType(style::PositionType position) {
    std::unordered_map<style::PositionType, YGPositionType> positions = {
        {style::PositionType::Absolute, YGPositionTypeAbsolute},
        {style::PositionType::Relative, YGPositionTypeRelative},
        {style::PositionType::Static, YGPositionTypeStatic},
    };

    YGNodeStyleSetPositionType(_yogaNode, positions[position]);
}

void Element::updateFlex(const style::Flex &flex) {
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

    if (auto optFlex = flex.flex)
        YGNodeStyleSetFlex(_yogaNode, *optFlex);

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

void Element::updateSpacing(const style::Spacing &spacing) {
    // margins

    std::unordered_map<YGEdge, std::optional<utils::ValueRatioAuto<int>>>
        margins = {{YGEdgeAll, spacing.margin},
                   {YGEdgeLeft, spacing.marginLeft},
                   {YGEdgeRight, spacing.marginRight},
                   {YGEdgeTop, spacing.marginTop},
                   {YGEdgeBottom, spacing.marginBottom},
                   {YGEdgeVertical, spacing.marginVertical},
                   {YGEdgeHorizontal, spacing.marginHorizontal}};

    for (auto &[edge, optMargin] : margins) {
        if (auto margin = optMargin) {
            if (auto marginValue = std::get_if<utils::Value<int>>(&*margin))
                YGNodeStyleSetMargin(_yogaNode, edge, marginValue->value);

            if (auto marginRatio = std::get_if<utils::Ratio>(&*margin))
                YGNodeStyleSetMarginPercent(
                    _yogaNode, edge, 100 * utils::clampRatio(marginRatio->ratio));

            if (std::get_if<utils::Auto>(&*margin))
                YGNodeStyleSetMarginAuto(_yogaNode, edge);
        }
    }

    // paddings

    std::unordered_map<YGEdge, std::optional<utils::ValueRatio<int>>> paddings = {
        {YGEdgeAll, spacing.padding},
        {YGEdgeLeft, spacing.paddingLeft},
        {YGEdgeRight, spacing.paddingRight},
        {YGEdgeTop, spacing.paddingTop},
        {YGEdgeBottom, spacing.paddingBottom},
        {YGEdgeHorizontal, spacing.paddingHorizontal},
        {YGEdgeVertical, spacing.paddingVertical}};

    for (auto &[edge, optPadding] : paddings) {
        if (auto padding = optPadding) {
            if (auto paddingValue = std::get_if<utils::Value<int>>(&*padding))
                YGNodeStyleSetPadding(_yogaNode, edge, paddingValue->value);

            if (auto paddingRatio = std::get_if<utils::Ratio>(&*padding))
                YGNodeStyleSetPaddingPercent(
                    _yogaNode, edge, 100 * utils::clampRatio(paddingRatio->ratio));
        }
    }

    // borders

    std::unordered_map<YGEdge, std::optional<float>> borders = {
        {YGEdgeAll, spacing.border},
        {YGEdgeLeft, spacing.borderLeft},
        {YGEdgeRight, spacing.borderRight},
        {YGEdgeTop, spacing.borderTop},
        {YGEdgeBottom, spacing.borderBottom}};

    for (auto &[edge, optBorder] : borders) {
        if (auto border = optBorder) {
            YGNodeStyleSetBorder(_yogaNode, edge, *border);
        }
    }
}

void Element::updateSize(const style::Size &size) {
    if (auto width = size.width) {
        if (auto value = std::get_if<utils::Value<int>>(&*width))
            YGNodeStyleSetWidth(_yogaNode, value->value);

        if (auto ratio = std::get_if<utils::Ratio>(&*width))
            YGNodeStyleSetWidthPercent(_yogaNode,
                                       100 * utils::clampRatio(ratio->ratio));

        if (std::holds_alternative<utils::Auto>(*width))
            YGNodeStyleSetWidthAuto(_yogaNode);
    }

    if (auto height = size.height) {
        if (auto value = std::get_if<utils::Value<int>>(&*height))
            YGNodeStyleSetHeight(_yogaNode, value->value);

        if (auto ratio = std::get_if<utils::Ratio>(&*height))
            YGNodeStyleSetHeightPercent(_yogaNode,
                                        100 * utils::clampRatio(ratio->ratio));

        if (std::holds_alternative<utils::Auto>(*height))
            YGNodeStyleSetHeightAuto(_yogaNode);
    }

    if (auto mw = size.minWidth)
        YGNodeStyleSetMinWidth(_yogaNode, *mw);

    if (auto mh = size.minHeight)
        YGNodeStyleSetMinHeight(_yogaNode, *mh);

    if (auto mw = size.maxWidth)
        YGNodeStyleSetMinWidth(_yogaNode, *mw);

    if (auto mh = size.maxHeight)
        YGNodeStyleSetMinHeight(_yogaNode, *mh);

    if (auto aspectRatio = size.aspectRatio)
        YGNodeStyleSetWidthPercent(_yogaNode,
                                   100 * utils::clampRatio(*aspectRatio));
}

void Element::drawBackground(const Rectangle &bb) {
    if (bb.width + bb.height == 0)
        return;

    if (auto bg = _style.backgroundColor) {
        if (auto radius = _style.borderRadius) {
            if (auto ratio = std::get_if<utils::Ratio>(&*radius)) {
                auto radius = utils::clampRatio(ratio->ratio);
                DrawRectangleRounded(bb, radius, getSegmentCount(radius), *bg);
            }

            if (auto value = std::get_if<utils::Value<float>>(&*radius)) {
                auto radius =
                    utils::clampRatio(value->value / std::min(bb.width, bb.height));
                DrawRectangleRounded(bb, radius, getSegmentCount(radius), *bg);
            }
        } else {
            DrawRectangle(bb.x, bb.y, bb.width, bb.height, *bg);
        }
    }
}

void Element::drawBorder(const Rectangle &bb) {
    if (!_layout.spacing.has_value())
        return;

    const auto evenBorderThickness = _layout.spacing->border.has_value() &&
                                     !_layout.spacing->borderBottom.has_value() &&
                                     !_layout.spacing->borderTop.has_value() &&
                                     !_layout.spacing->borderLeft.has_value() &&
                                     !_layout.spacing->borderRight.has_value();

    bool drawRoundedBorders = false;

    if (auto border = _layout.spacing->border;
        border.has_value() && evenBorderThickness) {
        if (auto radius = _style.borderRadius) {
            // TODO : make border radius
            // work on non-even borders
            if (auto borderColor = _style.borderColor) {
                const float halfBorder = *border <= 1.0 ? *border : (*border / 2);
                Rectangle rect = {
                    .x = bb.x - halfBorder,
                    .y = bb.y - halfBorder,
                    .width = bb.width + halfBorder,
                    .height = bb.height + halfBorder};

                if (auto ratio = std::get_if<utils::Ratio>(&*radius)) {
                    auto radius = utils::clampRatio(ratio->ratio);
                    DrawRectangleRoundedLines(rect, radius, getSegmentCount(radius),
                                              *border, *borderColor);
                }

                if (auto value = std::get_if<utils::Value<float>>(&*radius)) {
                    auto radius =
                        utils::clampRatio(value->value / std::min(rect.width, rect.height));
                    DrawRectangleRoundedLines(rect, radius, getSegmentCount(radius),
                                              *border, *borderColor);
                }

                drawRoundedBorders = true;
            }
        }
    }

    if (!drawRoundedBorders) {
        utils::Edges<Color> finalColors = {0};
        {
            if (auto bg = _style.borderColor) {
                finalColors.top = finalColors.left = finalColors.right =
                    finalColors.bottom = *bg;
            }

            if (auto colors = _style.borderColors) {
                if (auto top = colors->top)
                    finalColors.top = *top;
                if (auto bottom = colors->bottom)
                    finalColors.bottom = *bottom;
                if (auto left = colors->left)
                    finalColors.left = *left;
                if (auto right = colors->right)
                    finalColors.right = *right;
            }
        }

        utils::Edges<float> finalBorders = {0};
        {
            auto &spacing = *_layout.spacing;
            if (auto border = spacing.border) {
                finalBorders.top = finalBorders.left = finalBorders.right =
                    finalBorders.bottom = *border;
            }

            if (auto border = spacing.borderLeft)
                finalBorders.left = *border;

            if (auto border = spacing.borderTop)
                finalBorders.top = *border;

            if (auto border = spacing.borderBottom)
                finalBorders.bottom = *border;

            if (auto border = spacing.borderRight)
                finalBorders.right = *border;
        }

        if (finalBorders.bottom == finalBorders.top &&
            finalBorders.left == finalBorders.top &&
            finalBorders.left == finalBorders.right &&
            finalColors.bottom == finalColors.top &&
            finalColors.left == finalColors.top &&
            finalColors.left == finalColors.right) {

            const float border = finalBorders.left;
            const float halfBorder = border <= 1.0 ? border : (border / 2);
            Rectangle rect = {
                .x = bb.x - halfBorder,
                .y = bb.y - halfBorder,
                .width = bb.width + halfBorder,
                .height = bb.height + halfBorder};
            DrawRectangleLinesEx(rect, border, finalColors.top);
        } else
            utils::drawRectangle(bb, finalBorders.top, finalBorders.bottom,
                                 finalBorders.left, finalBorders.right, finalColors.top,
                                 finalColors.bottom, finalColors.left,
                                 finalColors.right);
    }
}

void Element::render(/* OFFSET GOES HERE */) {
    // TODO : render element at a given offset (provided offset)

    const auto bb = getBoundingRect();

    drawBackground(bb);
    drawBorder(bb);
}

std::shared_ptr<ui::rendering::StackingContext> Element::getParentStackingContext() const {
    if (auto parent = _parent.lock())
        return parent->getStackingContext();
    return nullptr;
}

void Element::setStackingContext(std::shared_ptr<ui::rendering::StackingContext> ctx) {
    _stackingContext = ctx;
}

std::shared_ptr<ui::rendering::StackingContext> Element::getStackingContext() const {
    return _stackingContext.lock();
}

int Element::getSegmentCount(float radius) const {
    return int(radius * 4) < 8 ? 8 : int(radius * 2.5);
}

ui::style::Theme Element::getPreferredTheme() const {
    return _preferredTheme;
}

void Element::setPreferredTheme(ui::style::Theme theme) {
    const auto prev = _preferredTheme;
    _preferredTheme = theme;

    if (prev != _preferredTheme) {
        onPreferredThemeChanged(_preferredTheme);
    }
}

void Element::setParent(std::shared_ptr<Element> parent) {
    _parent = parent;
    setPreferredTheme(parent->getPreferredTheme());
}

void Element::updateCachedInheritablePropsFrom(std::shared_ptr<Element> element) {
    if (!element)
        return;

    const auto &newProps = element->_cachedInheritableProps;
    _cachedInheritableProps.updateInheritedFields(_style.inheritables, newProps);
}

} // namespace element
} // namespace ui
