#include "gol.h"

#include <stdlib.h>
#include <time.h>

gol *
gol_init(int width, int height)
{
    srand(time(NULL));
    gol *g = malloc(sizeof(*g));
    g->read_matrix = malloc(sizeof(int) * width * height);
    g->write_matrix = malloc(sizeof(int) * width * height);
    g->width = width;
    g->height = height;
    return g;
}

void
gol_free(gol *g)
{
    free(g->read_matrix);
    free(g->write_matrix);
    free(g);
}

static inline int
get_cell(gol *g, int x, int y)
{
    return (x < 1 || x > g->width || y < 1 || y > g->height) ?
        0 : g->read_matrix[(x - 1) * g->height + y];
}

static inline void
set_cell(gol *g, int x, int y, int v)
{
    g->write_matrix[(x - 1) * g->height + y] = v;
}

static inline int
count_neighbours(gol *g, int y, int x)
{
    return get_cell(g, x - 1, y - 1) + get_cell(g, x, y - 1) +
           get_cell(g, x + 1, y - 1) + get_cell(g, x - 1, y) + 0 +
           get_cell(g, x + 1, y) + get_cell(g, x - 1, y + 1) +
           get_cell(g, x, y + 1) + get_cell(g, x + 1, y + 1);
}

void
gol_rand(gol *g)
{
    for (int y = 0; y < g->height; ++y)
        for (int x = 0; x < g->width; ++x)
            set_cell(g, x, y, rand() % 2);
}

void
gol_iter(gol *g)
{
    for (int y = 0; y < g->height; ++y) {
        for (int x = 0; x < g->width; ++x) {
            int count = count_neighbours(g, x, y);
            if (count == 3 || (count == 2 && get_cell(g, x, y) == 1))
                set_cell(g, x, y, 1);
            else
                set_cell(g, x, y, 0);
        }
    }
    int *m = g->write_matrix;
    g->write_matrix = g->read_matrix;
    g->read_matrix = m;
}

void
gol_draw(WINDOW *win, gol *g)
{
    for (int y = 0; y < g->height; ++y)
        for (int x = 0; x < g->width; ++x)
            if (get_cell(g, x, y))
                mvwaddch(win, y, x, '#');
            else
                mvwaddch(win, y, x, ' ');
}
