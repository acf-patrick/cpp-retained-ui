#include <raylib.h>
#include <ui.h>

#include <memory>

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 360

int main() {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Reatined UI with raylib");

  Vector2 windowSize = {.x = WINDOW_WIDTH, .y = WINDOW_HEIGHT};
  auto root = std::make_shared<ui::element::Root>(windowSize);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    root->render();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}