#include "./Image.h"
#include "../defaults.h"
#include "../repository/TextureRepository.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace ui {
namespace element {

Image::Image(const std::string &src, const std::string &alt) : Element("Image"), _src(src), _alt(alt) {
    auto textures = repository::TextureRepository::Get();
    if (!textures)
        throw std::logic_error("[Image] Texture repository not initialized.");

    updateStyle(ui::defaults::imageStyles());

    std::optional<Texture2D> texture = textures->get(src);
    if (!texture) {
        if (textures->load(src, src))
            texture = textures->get(src);
    }

    if (texture)
        updateLayout(ui::defaults::imageLayout({(float)texture->width, (float)texture->height}));
}

void Image::onChildAppended(std::shared_ptr<Element>) {
    throw std::logic_error("[Image] Image elemenet can only be used as leaf node.");
}

void Image::render() {
    auto textures = repository::TextureRepository::Get();
    if (!textures)
        throw std::logic_error("[Image] Texture repository not initialized.");

    auto optTexture = textures->get(_src);
    if (!optTexture) {
        drawAlt();
        return;
    }

    auto texture = *optTexture;
    auto bb = getBoundingRect();

    if (texture.width != bb.width || texture.height != bb.height) { // we have to scale and/or reposition image
        Rectangle src = {
                      .x = 0,
                      .y = 0,
                      .width = (float)texture.width,
                      .height = (float)texture.height},
                  dst = {0};
        const auto &props = *_style.drawableContentProps;

        switch (props.objectFit) {
        case ui::style::ObjectFit::Fill:
            dst = bb;
            break;

        case ui::style::ObjectFit::Cover: {
            const auto scale = std::max(bb.width / texture.width, bb.height / texture.height);
            dst = {
                .x = 0,
                .y = 0,
                .width = scale * texture.width,
                .height = scale * texture.height};
        } break;

        case ui::style::ObjectFit::Contain: {
            const auto scale = std::min(bb.width / texture.width, bb.height / texture.height);
            dst = {
                .x = 0,
                .y = 0,
                .width = scale * texture.width,
                .height = scale * texture.height};
        } break;

        case ui::style::ObjectFit::None: {
            dst = src;

            repositionDrawingRectangle(src);
        } break;

        case ui::style::ObjectFit::ScaleDown: {
        } break;
        }

        DrawTexturePro(texture, src, dst, {0}, 0.0, WHITE);
    } else {
        DrawTexture(texture, bb.x, bb.y, WHITE);
    }
}

void Image::drawAlt() {
    // TODO
}

void Image::repositionDrawingRectangles(Rectangle &src, Rectangle &dest, const float scale) {
    const auto bb = getBoundingRect();
    const auto &position = _style.drawableContentProps->objectPosition;

    if (std::holds_alternative<ui::style::ObjectPositionCenter>(position)) {
        Rectangle positionedSrc = {
            .x = bb.x + 0.5 * (bb.width - dest.width),
            .y = bb.y + 0.5 * (bb.height - dest.height),
            .width = dest.width,
            .height = dest.height};

        auto visible = GetCollisionRec(positionedSrc, bb);
        src = {(visible.x - positionedSrc.x) / scale,
               (visible.y - positionedSrc.y) / scale,
               visible.width / scale,
               visible.height / scale};

        dest = visible;
        dest.x -= bb.x;
        dest.y -= bb.y;

        return;
    }

    if (auto edges = std::get_if<ui::style::ObjectPositionEdge>(&position)) {
        switch (edges->x) {
        case ui::style::Edge::Left:

            break;
        case ui::style::Edge::Right:
            break;
        default:;
        }

        switch (edges->y) {
        case ui::style::Edge::Top:
            break;

        case ui::style::Edge::Bottom:
            break;
        default:;
        }

        return;
    }

    if (auto ratio = std::get_if<ui::style::ObjectPositionRatio>(&position)) {
        return;
    }

    if (auto positions = std::get_if<ui::style::ObjectPositionPosition>(&position)) {
        return;
    }
}

void Image::setSource(const std::string &src) { _src = src; }

void Image::setAlt(const std::string &alt) { _alt = alt; }

std::string Image::getSource() const { return _src; }

std::string Image::getAlt() const { return _alt; }

} // namespace element
} // namespace ui