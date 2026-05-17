#include "maze_solutions.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  u_vec2_t value;
  bool is_empty;
} next_path_t;

static int is_visited(maze_solution_t *mzs, u_vec2_t cell) {
  if (mzs == NULL)
    return -1;
  return mzs->visited[cell.y * mzs->maze->size.x + cell.x];
}

static void mark_as_visited(maze_solution_t *mzs, u_vec2_t cell) {
  if (mzs == NULL)
    return;
  mzs->visited[cell.y * mzs->maze->size.x + cell.x] = 1;
}

static next_path_t get_next_path(maze_solution_t *mzs) {
  u_vec2_t cell = mzs->mouse;
  maze_t *maze = mzs->maze;

  u_vec2_t available[4];
  size_t available_len = 0;

  const int directions[][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};

  for (size_t i = 0; i < 4; i++) {
    int dx = cell.x + directions[i][0];
    int dy = cell.y + directions[i][1];
    if (
      dx > 0 && dx < (int)maze->size.x - 1 && 
      dy > 0 && dy < (int)maze->size.y - 1 &&
      !is_visited(mzs, (u_vec2_t){dx, dy}) &&
      is_open_maze(maze, (u_vec2_t){dx, dy})
    )
      available[available_len++] = (u_vec2_t){dx, dy};
  }

  int ri = available_len > 1 ? rand() % available_len : 0;

  return (next_path_t){available[ri], !available_len};
}

maze_solution_t *init_maze_solution(maze_t *maze, u_vec2_t mouse) {
  if (maze == NULL)
    return NULL;

  maze_solution_t *mzs = calloc(1, sizeof(maze_solution_t));
  if (mzs == NULL)
    return NULL;

  mzs->visited = calloc(maze->size.x * maze->size.y, sizeof(int));
  if (mzs->visited == NULL) {
    free(mzs);
    return NULL;
  }

  mzs->path_len = 1;
  mzs->paths = calloc(maze->size.x * maze->size.y, sizeof(u_vec2_t));
  if (mzs->paths == NULL) {
    free(mzs->visited);
    free(mzs);
    return NULL;
  }

  mzs->mouse = mzs->paths[0] = mouse;
  mzs->maze = maze;
  mzs->is_end = false;

  return mzs;
}

void clear_maze_solution(maze_solution_t* mzs){
  memset(mzs->visited, 0, sizeof(int) * mzs->maze->size.x * mzs->maze->size.y);
  mzs->paths[0] = mzs->mouse = (u_vec2_t){0, 1};
  mzs->path_len = 1;
  mzs->is_end = false;
}

void free_maze_solution(maze_solution_t *mzs) {
  if (mzs == NULL)
    return;
  free(mzs->visited);
  free(mzs->paths);
  free(mzs);
}

void mouse_next_move(maze_solution_t *mzs) {
  if (mzs == NULL || mzs->is_end)
    return;

  mark_as_visited(mzs, mzs->mouse);
  next_path_t next_path = get_next_path(mzs);

  if (
    mzs->mouse.x == mzs->maze->size.x - 2 && 
    mzs->mouse.y == mzs->maze->size.y - 2
  ) {
    mzs->paths[mzs->path_len++] = mzs->mouse = (u_vec2_t){
      mzs->maze->size.x - 1, 
      mzs->maze->size.y - 2
    };
    mzs->is_end = true;
    return;
  }

  if (next_path.is_empty) {
    if (mzs->path_len) {
      mzs->paths[--mzs->path_len];
      mzs->mouse = mzs->paths[mzs->path_len - 1];
    }
    if (!mzs->is_end)
      mzs->is_end = !mzs->path_len;
    return;
  }

  mzs->paths[mzs->path_len++] = mzs->mouse = next_path.value;
}