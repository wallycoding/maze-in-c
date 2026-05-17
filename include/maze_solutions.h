#include "maze.h"
#include <raylib.h>

#ifndef MAZE_SOLUTIONS_H
#define MAZE_SOLUTIONS_H

typedef struct {
  maze_t *maze;
  u_vec2_t *paths;
  size_t path_len;
  cell_type_t *visited;
  u_vec2_t mouse;
  bool is_end;
} maze_solution_t;

maze_solution_t* init_maze_solution(maze_t* maze, u_vec2_t mouse);
void free_maze_solution(maze_solution_t* maze_solution);

void mouse_next_move(maze_solution_t* maze_solution);
void clear_maze_solution(maze_solution_t* maze_solution);

#endif
