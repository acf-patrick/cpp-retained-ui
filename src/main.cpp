#include <SFML/Graphics.hpp>
#include "./elements/Button.h"
#include "./styles/Style.h"

class RootElement : public element::Element {
public:
    RootElement(int windowWidth, int windowHeight): Element() {
        style::Style style;
        auto& size = style.size.emplace();
        size.width.emplace(windowWidth);
        size.height.emplace(windowHeight);

        auto& flex = style.flex.emplace();
        flex.flexDirection.emplace(style::FlexDirection::Row);

        YGNodeStyleSetGap(_yogaNode, YGGutterAll, 10);

        updateStyle(style);
    }

    void render(sf::RenderWindow& window) override {
        YGNodeCalculateLayout(_yogaNode, YGUndefined, YGUndefined, YGDirectionLTR);
        for (auto child: _children)
            child->render(window);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({ 480, 360 }), "Hello world!", sf::Style::Titlebar | sf::Style::Close);

    auto root = std::make_shared<RootElement>(window.getSize().x, window.getSize().y);

    style::Style buttonStyle;
    auto& size = buttonStyle.size.emplace();
    size.width = 72;
    size.height = 72;

    auto btn1 = std::make_shared<element::Button>();
    btn1->updateStyle(buttonStyle);
    root->appendChild(btn1);

    auto btn2 = std::make_shared<element::Button>();
    btn2->updateStyle(buttonStyle);
    root->appendChild(btn2);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        root->render(window);
        window.display();
    }

    return 0;
}