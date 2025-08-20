#pragma once

#include <string>
#include <raylib.h>

#include "./Element.h"

namespace ui {
namespace element {

class Image : public Element {
    std::string _src; // source image
    std::string _alt; // text to show if image file doesn't exist

  private:
    void drawAlt();
    void repositionDrawingRectangles(Rectangle& source, Rectangle& destination, const float scale);

    void onChildAppended(std::shared_ptr<Element>) override;

  public:
    Image(const std::string &src, const std::string &alt = "");

    std::string getAlt() const;
    void setAlt(const std::string &alt);

    std::string getSource() const;
    void setSource(const std::string &src);

    void render() override;
};

} // namespace element
} // namespace ui