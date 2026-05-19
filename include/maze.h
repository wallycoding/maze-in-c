#include <stddef.h>
#include <stdint.h>
#ifndef MAZE_H
#define MAZE_H

typedef struct {
  size_t x;
  size_t y;
} u_vec2_t;

// labirinto empacotado em bitset: cada cell gasta 1 bit em vez
// de um int inteiro (32 bits), 0 = wall, 1 = open
// 32x menos memoria -> cabe inteiro em cache facil
// _scratch eh a stack do DFS da geracao, alocada uma vez so no heap
// (antes era VLA na stack, quebrava em maze grande no windows)
typedef struct {
  uint64_t *bits;
  u_vec2_t size;
  u_vec2_t *_scratch;
} maze_t;

maze_t *init_maze(u_vec2_t size);
void free_maze(maze_t *maze);

void generate_maze(maze_t *maze);
void clear_maze(maze_t *maze);

// static inline no header -> o compilador cola direto no caller,
// sai shr + and sem call/ret. tirei o NULL check pq todos os
// callers ja garantem maze != NULL antes de chegar aq
static inline int is_open_maze(const maze_t *maze, u_vec2_t cell) {
  size_t idx = cell.y * maze->size.x + cell.x;
  return (int)((maze->bits[idx >> 6] >> (idx & 63)) & 1ULL);
}

#endif
