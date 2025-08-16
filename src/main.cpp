#include <raylib.h>
#include <repository.h>
#include <ui.h>
#include <iostream>
#include <memory>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reatined UI with raylib");
  SetTargetFPS(60);

  auto repositories = repository::InitRepositories();
  Vector2 windowSize = {.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT};
  auto root = std::make_shared<ui::element::Root>(windowSize);

  auto rootStyle = root->getStyle();
  rootStyle.flex->alignItems = ui::style::Alignment::Center;
  root->updateStyle(rootStyle);

  std::shared_ptr<ui::element::Element> btn[] = {
      std::make_shared<ui::element::Button>(YELLOW),
      std::make_shared<ui::element::Button>(BLUE),
  };

  ui::style::Style btnStyle;

  auto& size = btnStyle.size.emplace();
  size.width = utils::Value(46);
  size.height = utils::Value(46);

  btnStyle.positionType = ui::style::PositionType::Absolute;
  auto& position = btnStyle.position.emplace();
  position.left = utils::Value(0.0f);
  position.top = utils::Value(0.0f);

  btn[0]->updateStyle(btnStyle);
  // btn[1]->updateStyle(btnStyle);

  ui::element::Element::AppendChildren(root, btn[0], btn[1]);

  auto text = std::make_shared<ui::element::Text>("Retained UI with raylib");
  ui::element::Element::AppendChild(btn[1], text);

  while (!WindowShouldClose()) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      auto mousePos = GetMousePosition();
      auto btnStyle = btn[0]->getStyle();
      btnStyle.position->left = utils::Value(mousePos.x);
      btnStyle.position->top = utils::Value(mousePos.y);
      btn[0]->updateStyle(btnStyle);
    }

    BeginDrawing();
    if (root->shouldRecalculateLayout()) {
      root->calculateLayout();

      ClearBackground(BLACK);
      root->render();
    }
    EndDrawing();
  }

  repository::Repository::Clear(repositories);

  CloseWindow();
  return 0;
}