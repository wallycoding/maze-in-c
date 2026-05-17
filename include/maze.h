#include <stddef.h>
#ifndef MAZE_H
#define MAZE_H

typedef enum { MAZE_WALL, MAZE_PATH } cell_type_t;

typedef struct {
  size_t x;
  size_t y;
} u_vec2_t;

typedef struct {
  cell_type_t *cells;
  u_vec2_t size;
} maze_t;

maze_t *init_maze(u_vec2_t size);
void free_maze(maze_t *maze);

void generate_maze(maze_t *maze);
void clear_maze(maze_t *maze);
int is_open_maze(maze_t *maze, u_vec2_t cell);

#endif
