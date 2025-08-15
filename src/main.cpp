#include <raylib.h>
#include <ui.h>

#include <memory>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reatined UI with raylib");
  SetTargetFPS(60);

  Vector2 windowSize = {.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT};
  auto root = std::make_shared<ui::element::Root>(windowSize);

  std::shared_ptr<ui::element::Element> btn[] = {
    std::make_shared<ui::element::Button>(YELLOW),
    std::make_shared<ui::element::Button>(BLUE),
  };

  ui::style::Style btnStyle;

  auto& size = btnStyle.size.emplace();
  size.width.emplace(46);
  size.height.emplace(46);

  btn[0]->updateStyle(btnStyle);
  btn[1]->updateStyle(btnStyle);
  
  root->appendChild(btn[0]);
  root->appendChild(btn[1]);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    root->render();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}