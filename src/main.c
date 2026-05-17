#include "maze.h"
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 400
#define BLOCK 20

int main(void) {

  maze_t *maze = init_maze((u_vec2_t){WIDTH / BLOCK / 2 - 1, HEIGHT / BLOCK / 2 - 1});
  generate_maze(maze);

  InitWindow(WIDTH, HEIGHT, "Maze");

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_R)) {
      generate_maze(maze);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    for (size_t y = 0; y < maze->size.y; y++) {
      for (size_t x = 0; x < maze->size.x; x++) {

        if (is_open_maze(maze, (u_vec2_t){x, y}))
          continue;

        int pos_y = BLOCK / 2 + y * BLOCK;
        int pos_x = BLOCK / 2 + x * BLOCK;
        DrawRectangle(pos_x, pos_y, BLOCK, BLOCK, WHITE);
      }
    }

    EndDrawing();
  }

  CloseWindow();
  free_maze(maze);

  return 0;
}
