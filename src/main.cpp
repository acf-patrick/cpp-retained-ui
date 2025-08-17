#include <raylib.h>
#include <repository.h>
#include <ui.h>
#include <iostream>
#include <memory>

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

    auto& flex = layout.flex.emplace();
    flex.justifyContent = ui::style::JustifyContent::Center;
    flex.alignItems = ui::style::Alignment::Center;
    flex.flex = 1.0;

    view->updateLayout(layout);

    auto style = view->getStyle();
    style.backgroundColor = SKYBLUE;
    view->updateStyle(style);
  }

  auto button = std::make_shared<ui::element::Button>();
  ui::element::Element::AppendChild(view, button);

  auto text = std::make_shared<ui::element::Text>("This is a button");
  ui::element::Element::AppendChild(button, text);

  root->finalize();
  while (!WindowShouldClose()) {
    BeginDrawing();
    if (root->shouldRecalculateLayout()) {
      root->calculateLayout();
    }

    root->render();
    EndDrawing();
  }

  repository::Repository::Clear(repositories);

  CloseWindow();
  return 0;
}