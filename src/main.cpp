#include <raylib.h>
#include <repository.h>
#include <ui.h>
#include <iostream>
#include <memory>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reatined UI with raylib");
  SetTargetFPS(60);

  auto repositories = repository::InitRepositories();
  Vector2 windowSize = {.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT};
  auto root = std::make_shared<ui::element::Root>(windowSize);

  auto rootLayout = root->getLayout();
  auto& flex = rootLayout.flex.emplace();
  flex.justifyContent = ui::style::JustifyContent::Center;
  flex.alignItems = ui::style::Alignment::Center;
  root->updateLayout(rootLayout);

  auto button = std::make_shared<ui::element::Button>();
  ui::element::Element::AppendChild(root, button);

  auto btnStyle = button->getStyle();
  btnStyle.backgroundColor.emplace(DARKGRAY);
  btnStyle.borderColor.emplace(BLUE);
  btnStyle.borderRadius = utils::Value<float>(4);
  button->updateStyle(btnStyle);

  auto btnLayout = button->getLayout();
  auto& spacing = btnLayout.spacing.emplace();
  spacing.paddingVertical = utils::Value(5);
  spacing.paddingHorizontal = utils::Value(10);
  spacing.border = 3;
  button->updateLayout(btnLayout);

  auto text = std::make_shared<ui::element::Text>("Retained UI with raylib");
  ui::element::Element::AppendChild(button, text);

  while (!WindowShouldClose()) {
    BeginDrawing();
    if (root->shouldRecalculateLayout()) {
      root->calculateLayout();
    }

    ClearBackground(BLACK);
    root->render();
    EndDrawing();
  }

  repository::Repository::Clear(repositories);

  CloseWindow();
  return 0;
}