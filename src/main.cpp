#include <SFML/Graphics.hpp>

#include "./elements.h"
#include "./styles/Style.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({480, 360}), "Hello world!", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);

    auto root = std::make_shared<element::Root>(window.getSize());

    style::Style buttonStyle;
    auto &size = buttonStyle.size.emplace();
    size.width = 72;
    size.height = 72;

    auto btn1 = std::make_shared<element::Button>(sf::Color(255, 0, 0));
    btn1->updateStyle(buttonStyle);
    root->appendChild(btn1);

    auto btn2 = std::make_shared<element::Button>(sf::Color(255, 0, 255));
    btn2->updateStyle(buttonStyle);
    root->appendChild(btn2);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (auto resized = event->getIf<sf::Event::Resized>())
            {
                sf::FloatRect visibleArea(sf::Vector2f(0, 0), sf::Vector2f(resized->size));
                window.setView(sf::View(visibleArea));
            }
        }

        window.clear();
        root->render(window);
        window.display();
    }

    return 0;
}