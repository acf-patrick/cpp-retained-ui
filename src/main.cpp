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
  rootStyle.flex->alignItems.emplace(ui::style::Alignment::Center);
  root->updateStyle(rootStyle);

  std::shared_ptr<ui::element::Element> btn[] = {
      std::make_shared<ui::element::Button>(YELLOW),
      std::make_shared<ui::element::Button>(BLUE),
  };

  ui::style::Style btnStyle;

  auto& size = btnStyle.size.emplace();
  size.width.emplace(46);
  size.height.emplace(46);

  btn[0]->updateStyle(btnStyle);
  // btn[1]->updateStyle(btnStyle);

  ui::element::Element::AppendChildren(root, btn[0], btn[1]);

  auto text = std::make_shared<ui::element::Text>("Retained UI with raylib");
  ui::element::Element::AppendChild(btn[1], text);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    // todo compute only when dirty
    root->calculateLayout();
    root->render();
    EndDrawing();
  }

  repository::Repository::Clear(repositories);

  CloseWindow();
  return 0;
}