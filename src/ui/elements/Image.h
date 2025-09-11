#pragma once

#include <optional>
#include <raylib.h>
#include <string>

#include "./Element.h"

namespace ui {
namespace element {

class Image : public Element {
    std::string _src; // source image
    std::string _alt; // text to show if image file doesn't exist
    const Color _altColor;
    std::optional<Texture2D> _iconTexture;

  private:
    // Draw alternative text
    void drawAlt(const Vector2&);

    /**
     * Compute source and destination rects to draw final image
     * @param src Initialized at position 0 with real texture size
     * @param dest Initialized at position 0 with image final size (scale)
     * @param scale Scale factor between `dest` and `src`
     */
    void repositionDrawingRectangles(Rectangle &src, Rectangle &dest, const float scale);

    void loadAltImageIconTexture();
    void onChildAppended(std::shared_ptr<Element>) override;

  public:
    Image(const std::string &src, const std::string &alt = "");
    ~Image();

    std::string getAlt() const;
    void setAlt(const std::string &alt);

    std::string getSource() const;
    void setSource(const std::string &src);

    void render(const Vector2&) override;
};

} // namespace element
} // namespace ui