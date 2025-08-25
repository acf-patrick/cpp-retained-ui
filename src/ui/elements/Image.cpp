#include "./Image.h"
#include "../defaults.h"
#include "../icons.h"
#include "../core/repository/TextureRepository.h"
#include "../utils/debug.h"

#include <iostream>

namespace ui {
namespace element {

Image::Image(const std::string &src, const std::string &alt) : Element("Image"), _src(src), _alt(alt), _altColor(Color{.r = 0x8A, .g = 0x8A, .b = 0x8A, .a = 0xff}) {
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

Image::~Image() {
    if (_iconTexture)
        UnloadTexture(*_iconTexture);
}

void Image::onChildAppended(std::shared_ptr<Element>) {
    throw std::logic_error("[Image] Image elemenet can only be used as leaf node.");
}

void Image::loadAltImageIconTexture() {
    if (_iconTexture)
        return;

    auto icon = LoadImageFromMemory(".png", ui::icon::ImagePng, ui::icon::ImagePngLen);
    _iconTexture = LoadTextureFromImage(icon);
    UnloadImage(icon);
}

void Image::render() {
    auto textures = repository::TextureRepository::Get();
    if (!textures)
        throw std::logic_error("[Image] Texture repository not initialized.");

    auto optTexture = textures->get(_src);
    if (!optTexture) {
        loadAltImageIconTexture();
        drawAlt();
        return;
    }

    Element::render();

    auto texture = *optTexture;
    auto bb = getBoundingRect();

    if (texture.width != bb.width || texture.height != bb.height) { // we have to scale and/or reposition image
        Rectangle src = {
                      .x = 0,
                      .y = 0,
                      .width = (float)texture.width,
                      .height = (float)texture.height},
                  dest = {0};
        const auto &props = *_style.drawableContentProps;

        switch (props.objectFit) {
        case ui::style::ObjectFit::Fill:
            dest = bb;
            break;

        case ui::style::ObjectFit::Cover: {
            const auto scale = std::max(bb.width / texture.width, bb.height / texture.height);
            dest.width = scale * texture.width;
            dest.height = scale * texture.height;
            repositionDrawingRectangles(src, dest, scale);
        } break;

        case ui::style::ObjectFit::Contain: {
            const auto scale = std::min(bb.width / texture.width, bb.height / texture.height);
            dest.width = scale * texture.width;
            dest.height = scale * texture.height;
            // std::cout << "scale : " << scale << std::endl;
            repositionDrawingRectangles(src, dest, scale);
        } break;

        case ui::style::ObjectFit::None:
            dest = src;
            repositionDrawingRectangles(src, dest, 1.0);
            break;

        case ui::style::ObjectFit::ScaleDown: {
            const auto scale = std::min(bb.width / texture.width, bb.height / texture.height);
            if (scale >= 1.0) { // use NONE
                dest = src;
                repositionDrawingRectangles(src, dest, 1.0);
            } else { // use CONTAIN
                dest.width = scale * texture.width;
                dest.height = scale * texture.height;
                repositionDrawingRectangles(src, dest, scale);
            }
        } break;
        }

        DrawTexturePro(texture, src, dest, {0}, 0.0, WHITE);
    } else {
        DrawTexture(texture, bb.x, bb.y, WHITE);
    }
}

void Image::drawAlt() {
    loadAltImageIconTexture();
    const auto bb = getBoundingRect();
    const int margin = 8;

    DrawTexture(*_iconTexture, bb.x, bb.y, WHITE);
    DrawText(_alt.c_str(), bb.x + _iconTexture->width + margin, bb.y, 16, _altColor);
}

void Image::repositionDrawingRectangles(Rectangle &src, Rectangle &dest, const float scale) {
    const auto bb = getBoundingRect();
    Rectangle positionedSrc = {0};
    const auto &position = _style.drawableContentProps->objectPosition;

    if (std::holds_alternative<ui::style::ObjectPositionCenter>(position)) {
        positionedSrc = {
            .x = float(bb.x + 0.5 * (bb.width - dest.width)),
            .y = float(bb.y + 0.5 * (bb.height - dest.height)),
            .width = dest.width,
            .height = dest.height};
    } else {
        if (auto edges = std::get_if<ui::style::ObjectPositionEdge>(&position)) {
            switch (edges->x) {
            case ui::style::Edge::Center:
                positionedSrc.x = bb.x + 0.5 * (bb.width - dest.width);
                break;

            case ui::style::Edge::Left:
                positionedSrc.x = bb.x;
                break;

            case ui::style::Edge::Right:
                positionedSrc.x = bb.x + bb.width - dest.width;
                break;
            default:;
            }

            switch (edges->y) {
            case ui::style::Edge::Center:
                positionedSrc.y = bb.y + 0.5 * (bb.height - dest.height);
                break;

            case ui::style::Edge::Top:
                positionedSrc.y = bb.y;
                break;

            case ui::style::Edge::Bottom:
                positionedSrc.y = bb.y + bb.height - dest.height;
                break;
            default:;
            }
        } else if (auto ratio = std::get_if<ui::style::ObjectPositionRatio>(&position)) {
            positionedSrc.x = bb.x + ratio->x * bb.width;
            positionedSrc.y = bb.y + ratio->y * bb.height;
        } else if (auto positions = std::get_if<ui::style::ObjectPositionPosition>(&position)) {
            positionedSrc.x = bb.x + positions->x;
            positionedSrc.y = bb.y + positions->y;
        }

        positionedSrc.width = dest.width;
        positionedSrc.height = dest.height;
    }

    dest = GetCollisionRec(positionedSrc, bb);
    src = {(dest.x - positionedSrc.x) / scale,
           (dest.y - positionedSrc.y) / scale,
           dest.width / scale,
           dest.height / scale};

    // std::cout << "dest : " << dest.x << ", " << dest.y << ", " << dest.width << ", " << dest.height << std::endl;
    // std::cout << "src : " << src.x << ", " << src.y << ", " << src.width << ", " << src.height << std::endl;
}

void Image::setSource(const std::string &src) { _src = src; }

void Image::setAlt(const std::string &alt) { _alt = alt; }

std::string Image::getSource() const { return _src; }

std::string Image::getAlt() const { return _alt; }

} // namespace element
} // namespace ui