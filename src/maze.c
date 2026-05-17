#include "maze.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  u_vec2_t value;
  bool is_empty;
} next_path_t;

static next_path_t get_next_path(maze_t *maze, u_vec2_t cell) {
  u_vec2_t available[4];
  size_t available_len = 0;

  const int directions[][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};

  for (size_t i = 0; i < 4; i++) {
    int dx = cell.x + 2 * directions[i][0];
    int dy = cell.y + 2 * directions[i][1];
    if (dx > 0 && dx < (int)maze->size.x - 1 && dy > 0 && dy < (int)maze->size.y - 1 &&
        !is_open_maze(maze, (u_vec2_t){dx, dy}))
      available[available_len++] = (u_vec2_t){dx, dy};
  }

  int ri = available_len > 1 ? rand() % available_len : 0;

  return (next_path_t){available[ri], !available_len};
}

static void open_maze(maze_t *maze, u_vec2_t cell) {
  if (maze == NULL)
    return;
  maze->cells[cell.y * maze->size.x + cell.x] = MAZE_PATH;
}

maze_t *init_maze(u_vec2_t size) {

  maze_t *maze = calloc(1, sizeof(maze_t));
  if (maze == NULL)
    return NULL;

  maze->size.x = size.x * 2 + 1;
  maze->size.y = size.y * 2 + 1;

  maze->cells = calloc(maze->size.x * maze->size.y, sizeof(int));

  if (maze->cells == NULL) {
    free(maze);
    return NULL;
  }

  return maze;
}

void free_maze(maze_t *maze) {
  if (maze == NULL)
    return;
  free(maze->cells);
  free(maze);
}

/**
 * the generation of the maze must be a srand(time(NULL))
 */
void generate_maze(maze_t *maze) {

  if (maze == NULL)
    return;

  clear_maze(maze);
  open_maze(maze, (u_vec2_t){0, 1});
  open_maze(maze, (u_vec2_t){maze->size.x - 1, maze->size.y - 2});

  u_vec2_t paths[maze->size.x * maze->size.y];
  paths[0] = (u_vec2_t){1, 1};
  size_t path_len = 1;

  while (path_len) {
    u_vec2_t path = paths[path_len - 1];
    next_path_t next_path = get_next_path(maze, path);
    open_maze(maze, path);

    if (next_path.is_empty) {
      path_len--;
      continue;
    }

    open_maze(maze, (u_vec2_t){(path.x + next_path.value.x) / 2, (path.y + next_path.value.y) / 2});
    paths[path_len++] = next_path.value;
  }
}

void clear_maze(maze_t *maze) {
  if (maze == NULL)
    return;
  memset(maze->cells, MAZE_WALL, sizeof(int) * maze->size.x * maze->size.y);
}

int is_open_maze(maze_t *maze, u_vec2_t cell) {
  if (maze == NULL)
    return -1;
  return maze->cells[cell.y * maze->size.x + cell.x] == MAZE_PATH;
}
