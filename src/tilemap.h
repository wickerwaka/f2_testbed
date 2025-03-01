#if !defined(TILEMAP_H)
#define TILEMAP_H 1

#include <stdint.h>

typedef enum
{
    BG0 = 0,
    BG1,
    FG0,
} Layer;

void on_layer(Layer layer);
void pen_color(int color);
void move_to(int x, int y);
void print(const char *fmt, ...);
void print_at(int x, int y, const char *fmt, ...);
void sym_at(int x, int y, uint16_t sym);

#endif
