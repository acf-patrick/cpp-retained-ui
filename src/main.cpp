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

        auto style = view->getStyle();
        // style.backgroundColor = SKYBLUE;
        view->updateStyle(style);
    }

    auto button = std::make_shared<ui::element::Button>();
    {
        auto style = button->getStyle();
        style.inheritables.color = YELLOW;
        button->updateStyle(style);
    }
    ui::element::Element::AppendChild(view, button);

    auto text = std::make_shared<ui::element::Text>("This is a button");
    ui::element::Element::AppendChild(button, text);

    root->finalize();
    while (!WindowShouldClose()) {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            auto style = button->getStyle();
            style.inheritables.color = style.inheritables.color == WHITE ? YELLOW : WHITE;
            button->updateStyle(style);
        }

        root->update();
        BeginDrawing();
        root->render();
        EndDrawing();
    }

    repository::Repository::Clear(repositories);

    CloseWindow();
    return 0;
}