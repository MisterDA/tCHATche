#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <ncurses.h>

typedef struct {
    int *read_matrix;
    int *write_matrix;
    int width, height;
} gol;

gol *gol_init(int width, int height);
void gol_free(gol *g);

void gol_rand(gol *g);
void gol_iter(gol *g);
void gol_draw(WINDOW *win, gol *g);

#endif
