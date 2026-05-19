#include "maze.h"
#include "maze_solutions.h"
#include "mouse.h"

#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 999
#define HEIGHT 999
#define BLOCK 4

int main(void) {

  srand(time(NULL));

  // Initialize Maze
  maze_t *maze = init_maze((u_vec2_t){WIDTH / BLOCK / 2 - 1, HEIGHT / BLOCK / 2 - 1});
  int spx = (WIDTH - maze->size.x * BLOCK) / 2;
  int spy = (HEIGHT - maze->size.y * BLOCK) / 2;

  generate_maze(maze);

  // Initialize Maze Solutions

  maze_solution_t *maze_solution = init_maze_solution(maze, (u_vec2_t){0, 1});
  float mouse_speed = 0;

  // Drawing

  InitWindow(WIDTH, HEIGHT, "Maze");

  while (!WindowShouldClose()) {

    if (IsKeyPressed(KEY_R)) {
      clear_maze_solution(maze_solution);
      if (!IsKeyDown(KEY_LEFT_CONTROL)) generate_maze(maze);
    }

    BeginDrawing();
    ClearBackground(BLACK);

    for (size_t y = 0; y < maze->size.y; y++) {
      for (size_t x = 0; x < maze->size.x; x++) {

        if (is_open_maze(maze, (u_vec2_t){x, y}))
          continue;

        int pos_y = spy + y * BLOCK;
        int pos_x = spx + x * BLOCK;
        DrawRectangle(pos_x, pos_y, BLOCK, BLOCK, WHITE);
      }
    }

    for (size_t i = 0; i < maze_solution->path_len; i++) {
      DrawCircle(
        spx + maze_solution->paths[i].x * BLOCK + (BLOCK/2),
        spy + maze_solution->paths[i].y * BLOCK + (BLOCK/2),
        BLOCK/4,
        BLUE
      );
    }

    draw_mouse((Vector2){
      spx + maze_solution->mouse.x * BLOCK, 
      spy + maze_solution->mouse.y * BLOCK
    }, BLOCK);

    mouse_speed += GetFrameTime() * 25;
    if (mouse_speed > 1.0f) {
      mouse_next_move(maze_solution);
      mouse_speed = 0;
    }

    EndDrawing();
  }

  CloseWindow();
  free_maze_solution(maze_solution);
  free_maze(maze);

  return 0;
}
