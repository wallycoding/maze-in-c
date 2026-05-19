#include "maze.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  u_vec2_t value;
  bool is_empty;
} next_path_t;

// xorshift32: 3 shifts + 3 xors, sem mutex, ~5x mais rapido que
// o rand() do libc (que trava lock por chamada). pra escolher 1
// de 4 direcoes a qualidade eh mais que suficiente
static uint32_t xs_state = 0;
static inline uint32_t xs_rand(void) {
  if (!xs_state) xs_state = ((uint32_t)time(NULL) ^ 0xCAFE5EEDu) | 1u;
  uint32_t s = xs_state;
  s ^= s << 13;
  s ^= s >> 17;
  s ^= s << 5;
  return xs_state = s;
}

// directions no escopo do arquivo, nao realocada na stack toda chamada
static const int dirs[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};

// quantos uint64 a gnt precisa pra empacotar todas as cells
// (cada um segura 64 bits). arredonda pra cima com o +63
static inline size_t maze_word_count(const maze_t *m) {
  return (m->size.x * m->size.y + 63) >> 6;
}

// liga o bit da cell -> ela vira path. inlinada e sem NULL check,
// callers garantem maze != NULL. idx >> 6 = qual word, idx & 63 = qual bit
static inline void open_maze(maze_t *maze, u_vec2_t cell) {
  size_t idx = cell.y * maze->size.x + cell.x;
  maze->bits[idx >> 6] |= (uint64_t)1 << (idx & 63);
}

static next_path_t get_next_path(maze_t *maze, u_vec2_t cell) {
  u_vec2_t available[4];
  size_t available_len = 0;

  // hoist das bordas pra fora do loop, evita reler do struct 4x
  const int xmax = (int)maze->size.x - 1;
  const int ymax = (int)maze->size.y - 1;
  const int cx = (int)cell.x;
  const int cy = (int)cell.y;

  for (size_t i = 0; i < 4; i++) {
    int dx = cx + 2 * dirs[i][0];
    int dy = cy + 2 * dirs[i][1];
    if (dx > 0 && dx < xmax && dy > 0 && dy < ymax &&
        !is_open_maze(maze, (u_vec2_t){dx, dy}))
      available[available_len++] = (u_vec2_t){dx, dy};
  }

  uint32_t ri = available_len > 1 ? xs_rand() % (uint32_t)available_len : 0;

  return (next_path_t){available[ri], !available_len};
}

maze_t *init_maze(u_vec2_t size) {

  maze_t *maze = calloc(1, sizeof(maze_t));
  if (maze == NULL)
    return NULL;

  maze->size.x = size.x * 2 + 1;
  maze->size.y = size.y * 2 + 1;

  // aloca o bitset, 32x menos coisa que o int[] antigo
  maze->bits = calloc(maze_word_count(maze), sizeof(uint64_t));
  if (maze->bits == NULL) {
    free(maze);
    return NULL;
  }

  // stack do DFS pre-alocada no heap. uma vez so, em vez de VLA
  // alocada a cada generate_maze (que ainda crashava em maze grande)
  maze->_scratch = malloc(maze->size.x * maze->size.y * sizeof(u_vec2_t));
  if (maze->_scratch == NULL) {
    free(maze->bits);
    free(maze);
    return NULL;
  }

  return maze;
}

void free_maze(maze_t *maze) {
  if (maze == NULL)
    return;
  free(maze->_scratch);
  free(maze->bits);
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

  // usa o scratch pre-alocado em init_maze, zero VLA
  u_vec2_t *paths = maze->_scratch;
  paths[0] = (u_vec2_t){1, 1};
  open_maze(maze, paths[0]);   // abre na hora de empilhar -> uma vez so
  size_t path_len = 1;

  while (path_len) {
    u_vec2_t path = paths[path_len - 1];
    next_path_t next_path = get_next_path(maze, path);

    if (next_path.is_empty) {
      path_len--;
      continue;
    }

    // abre a parede entre as duas + a propria cell nova de uma vez,
    // assim get_next_path nunca mais reconsidera ela como candidata
    // e ainda evita bit-set redundante a cada volta do backtrack
    open_maze(maze, (u_vec2_t){(path.x + next_path.value.x) / 2, (path.y + next_path.value.y) / 2});
    open_maze(maze, next_path.value);
    paths[path_len++] = next_path.value;
  }
}

// zera o bitset todo de uma vez so. como agr o estado "wall" eh
// o bit 0, memset(0) funciona sem depender de valor de enum
void clear_maze(maze_t *maze) {
  if (maze == NULL)
    return;
  memset(maze->bits, 0, maze_word_count(maze) * sizeof(uint64_t));
}

// is_open_maze foi pro header como static inline -> compilador
// inlinea direto no caller, sem call/ret
