#include "maze_solutions.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  u_vec2_t value;
  bool is_empty;
} next_path_t;

// xorshift32 -> bem mais rapido que rand() do libc, sem mutex.
// state separado do maze.c (cada TU sua), seed diferente pra nao
// pegarem mesma sequencia caso ambos seedem no mesmo segundo
static uint32_t xs_state = 0;
static inline uint32_t xs_rand(void) {
  if (!xs_state) xs_state = ((uint32_t)time(NULL) ^ 0xBEEFF00Du) | 1u;
  uint32_t s = xs_state;
  s ^= s << 13;
  s ^= s >> 17;
  s ^= s << 5;
  return xs_state = s;
}

// directions no escopo do arquivo, fora da stack do hot loop
static const int dirs[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};

// quantas words de 64 bit pro bitset de visitados
static inline size_t visited_word_count(const maze_solution_t *mzs) {
  return (mzs->maze->size.x * mzs->maze->size.y + 63) >> 6;
}

// inlinada, sem NULL check. callers garantem mzs != NULL
static inline int is_visited(const maze_solution_t *mzs, u_vec2_t cell) {
  size_t idx = cell.y * mzs->maze->size.x + cell.x;
  return (int)((mzs->visited[idx >> 6] >> (idx & 63)) & 1ULL);
}

static inline void mark_as_visited(maze_solution_t *mzs, u_vec2_t cell) {
  size_t idx = cell.y * mzs->maze->size.x + cell.x;
  mzs->visited[idx >> 6] |= (uint64_t)1 << (idx & 63);
}

static next_path_t get_next_path(maze_solution_t *mzs) {
  u_vec2_t cell = mzs->mouse;
  maze_t *maze = mzs->maze;

  u_vec2_t available[4];
  size_t available_len = 0;

  // hoist das bordas e da pos atual pra fora do loop
  const int xmax = (int)maze->size.x - 1;
  const int ymax = (int)maze->size.y - 1;
  const int cx = (int)cell.x;
  const int cy = (int)cell.y;

  for (size_t i = 0; i < 4; i++) {
    int dx = cx + dirs[i][0];
    int dy = cy + dirs[i][1];
    if (
      dx > 0 && dx < xmax &&
      dy > 0 && dy < ymax &&
      !is_visited(mzs, (u_vec2_t){dx, dy}) &&
      is_open_maze(maze, (u_vec2_t){dx, dy})
    )
      available[available_len++] = (u_vec2_t){dx, dy};
  }

  uint32_t ri = available_len > 1 ? xs_rand() % (uint32_t)available_len : 0;

  return (next_path_t){available[ri], !available_len};
}

maze_solution_t *init_maze_solution(maze_t *maze, u_vec2_t mouse) {
  if (maze == NULL)
    return NULL;

  maze_solution_t *mzs = calloc(1, sizeof(maze_solution_t));
  if (mzs == NULL)
    return NULL;

  // bitset de visitados: 1 bit por cell -> 32x menor q int[]
  size_t words = (maze->size.x * maze->size.y + 63) >> 6;
  mzs->visited = calloc(words, sizeof(uint64_t));
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
  // zera o bitset inteirinho de uma vez (32x menos bytes pra escrever)
  memset(mzs->visited, 0, visited_word_count(mzs) * sizeof(uint64_t));
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
