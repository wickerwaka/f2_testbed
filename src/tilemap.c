#include <stdint.h>
#include <stdarg.h>

#include "printf/printf.h"
#include "tilemap.h"
#include "system.h"

#define SCN TC0100SCN

Layer cur_layer;
uint16_t cur_x, cur_y;
uint16_t cur_color;

void on_layer(Layer layer)
{
    cur_layer = layer;
}

void pen_color(int color)
{
    cur_color = color;
}

void move_to(int x, int y)
{
    cur_x = x;
    cur_y = y;
}

static void print_string(const char *str)
{
    uint16_t x = cur_x;
    uint16_t y = cur_y;

    uint16_t ofs = ( y * 64 ) + x;

    uint16_t attr_color = (0 << 8) | (cur_color & 0xff);

    while(*str)
    {
        if( *str == '\n' )
        {
            y++;
            x = cur_x;
            ofs = (y * 64) + x;
        }
        else
        {
            switch(cur_layer)
            {
                case BG0:
                    SCN->bg0[ofs].attr_color = attr_color;
                    SCN->bg0[ofs].code = *str;
                    break;

                case BG1:
                    SCN->bg1[ofs].attr_color = attr_color;
                    SCN->bg1[ofs].code = *str;
                    break;

                case FG0:
                    SCN->fg0[ofs].attr = cur_color & 0x3f;
                    SCN->fg0[ofs].code = *str;
                    break;
                default:
                    break;
            }
            ofs++;
            x++;
        }
        str++;
    }
    cur_x = x;
    cur_y = y;
}

void print(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    buf[127] = '\0';
    va_end(args);
    
    print_string(buf);
}

void print_at(int x, int y, const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    buf[127] = '\0';
    va_end(args);
    
    move_to(x, y);
    print_string(buf);
}

void sym_at(int x, int y, uint16_t sym)
{
    uint16_t ofs = ( y * 64 ) + x;
    uint16_t attr_color = ( 0 << 8 ) | ( cur_color & 0xff );
    
    switch(cur_layer)
    {
        case BG0:
            SCN->bg0[ofs].attr_color = attr_color;
            SCN->bg0[ofs].code = sym;
            break;

        case BG1:
            SCN->bg1[ofs].attr_color = attr_color;
            SCN->bg1[ofs].code = sym;
            break;

        case FG0:
            SCN->fg0[ofs].attr = cur_color & 0x3f;
            SCN->fg0[ofs].code = sym;
            break;
        default:
            break;
    }
}

