#include <iostream>
#include <memory>
#include <raylib.h>
#include <repository.h>
#include <ui.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main() {
    // TEST PLAYGROUND

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Retained UI with raylib");

    SetTargetFPS(60);

    auto repositories = repository::InitRepositories();
    Vector2 windowSize = {.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT};
    auto root = std::make_shared<ui::element::Root>(windowSize);

    auto view = std::make_shared<ui::element::View>();
    ui::element::Element::AppendChild(root, view);

    {
        auto layout = view->getLayout();
        auto &flex = layout.flex.emplace();
        flex.justifyContent = ui::style::JustifyContent::Center;
        flex.alignItems = ui::style::Alignment::Center;
        flex.flex = 1.0;
        view->updateLayout(layout);
    }

    auto button = std::make_shared<ui::element::Button>();
    // ui::element::Element::AppendChild(view, button);

    {
        auto style = button->getStyle();
        style.inheritables.fontSize = 24;
        style.inheritables.color = BROWN;
        std::vector<std::string> fontFamily;
        fontFamily.push_back("roboto");
        style.inheritables.fontFamily = fontFamily;
        button->updateStyle(style);
    }

    auto text = std::make_shared<ui::element::Text>("This is a button");
    ui::element::Element::AppendChild(button, text);

    repository::FontRepository::Get()->load("roboto", "Roboto-Regular.ttf");

    auto imgContainer = std::make_shared<ui::element::View>();
    ui::element::Element::AppendChild(view, imgContainer);
    {
        auto layout = imgContainer->getLayout();
        layout.spacing.emplace().border = 3;
        auto &size = layout.size.emplace();
        size.width = utils::Value(480);
        size.height = utils::Value(300);
        imgContainer->updateLayout(layout);

        auto style = imgContainer->getStyle();
        style.borderColor = WHITE;
        imgContainer->updateStyle(style);
    }

    auto image = std::make_shared<ui::element::Image>("assets/images/cat.pg", "cat");
    ui::element::Element::AppendChild(imgContainer, image);
    {
        auto style = image->getStyle();
        auto &props = *style.drawableContentProps;
        props.objectFit = ui::style::ObjectFit::ScaleDown;
        props.objectPosition = ui::style::ObjectPositionCenter{};
        image->updateStyle(style);

        auto layout = image->getLayout();
        auto &size = layout.size.emplace();
        size.width = utils::Ratio(1.0);
        size.height = utils::Ratio(1.0);

        image->updateLayout(layout);
    }

    root->finalize();
    while (!WindowShouldClose()) {
        root->update();
        BeginDrawing();
        root->render();
        EndDrawing();
    }

    repository::Repository::Clear(repositories);

    CloseWindow();
    return 0;
}