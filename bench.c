#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

int main(int argc, char **argv) {
    int n_iter   = argc > 1 ? atoi(argv[1]) : 50;
    int half     = argc > 2 ? atoi(argv[2]) : 499; // 499 -> 999x999

    maze_t *maze = init_maze((u_vec2_t){half, half});
    if (!maze) { fprintf(stderr, "init failed\n"); return 1; }

    // warm-up (cache, allocations, etc)
    generate_maze(maze);

    double total = 0, mn = 1e18, mx = 0;
    for (int i = 0; i < n_iter; i++) {
        double t0 = now_ms();
        generate_maze(maze);
        double dt = now_ms() - t0;
        total += dt;
        if (dt < mn) mn = dt;
        if (dt > mx) mx = dt;
    }

    printf("iters=%d  size=%zux%zu  cells=%zu\n",
           n_iter, maze->size.x, maze->size.y,
           maze->size.x * maze->size.y);
    printf("avg=%.4f ms  min=%.4f ms  max=%.4f ms  total=%.2f ms\n",
           total / n_iter, mn, mx, total);

    free_maze(maze);
    return 0;
}
