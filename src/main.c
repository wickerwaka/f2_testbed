#include <stdint.h>
#include <stdbool.h>
#include "printf/printf.h"

#include "util.h"
#include "interrupts.h"
#include "tilemap.h"
#include "tc0100scn.h"
#include "tc0220ioc.h"

#include "palette.h"

volatile uint16_t *TC011PCR_ADDR = (volatile uint16_t *)0x200000;
volatile uint16_t *TC011PCR_DATA = (volatile uint16_t *)0x200002;
volatile uint16_t *TC011PCR_WHAT = (volatile uint16_t *)0x200004;

TC0100SCN_Layout *TC0100SCN = (TC0100SCN_Layout *)0x800000;
TC0100SCN_Control *TC0100SCN_Ctrl = (TC0100SCN_Control *)0x820000;

TC0220IOC_Control *TC0220IOC = (TC0220IOC_Control *)0x300000;

uint16_t *TC0200OBJ = (uint16_t *)0x900000;

void set_color(int index, uint8_t r, uint8_t g, uint8_t b)
{
    if (index >= 0x1000) return;

    uint16_t color = ((r >> 3) << 0) | ((g >> 3) << 5) | ((b >> 3) << 10);
    
    *TC011PCR_ADDR = index * 2;
    *TC011PCR_DATA = color;
}

void set_16color_palette(int index, uint8_t r, uint8_t g, uint8_t b)
{
    int color_index = index * 16;
    set_color(color_index, 0, 0, 0);
    for( int i = 1; i < 16; i++ )
    {
        set_color(color_index + i, r, g, b);
    }
}

void set_colors(uint16_t offset, uint16_t count, uint16_t *colors)
{
    for( uint16_t i = 0; i < count; i++ )
    {
        *TC011PCR_ADDR = (offset + i) * 2;
        *TC011PCR_DATA = colors[i];
    }
}



void draw_bg_text(TC0100SCN_BG *bg, int color, uint16_t x, uint16_t y, const char *str)
{
    int ofs = ( y * 64 ) + x;

    while(*str)
    {
        if( *str == '\n' )
        {
            y++;
            ofs = (y * 64) + x;
        }
        else
        {
            bg[ofs].attr = 0;
            bg[ofs].color = color & 0xff;
            bg[ofs].code = *str;
            ofs++;
        }
        str++;
    }
}

void draw_fg_text(TC0100SCN_FG *fg, int color, uint16_t x, uint16_t y, const char *str)
{
    int ofs = ( y * 64 ) + x;

    while(*str)
    {
        if( *str == '\n' )
        {
            y++;
            ofs = (y * 64) + x;
        }
        else
        {
            fg[ofs].attr = color & 0x3f;
            fg[ofs].code = *str;
            ofs++;
        }
        str++;
    }
}


volatile uint32_t vblank_count = 0;
volatile uint32_t dma_count = 0;

void level5_handler()
{
    vblank_count++;
    TC0220IOC->watchdog = 0;
}

void level6_handler()
{
    dma_count++;
}

extern char _binary_src_font_chr_start[];
extern char _binary_src_font_chr_end[];

int main(int argc, char *argv[])
{
    uint16_t edge_count = 0;

    for( int x = 0; x < 0x8000; x++ )
    {
        TC0200OBJ[x] = 0;
    }

    memset(TC0100SCN, 0, sizeof(TC0100SCN_Layout));

    TC0100SCN_Ctrl->bg1_y = 8;
    TC0100SCN_Ctrl->bg1_x = 17;
    TC0100SCN_Ctrl->fg0_y = 8;
    TC0100SCN_Ctrl->fg0_x = 17;
    TC0100SCN_Ctrl->system_flags = 0;
    TC0100SCN_Ctrl->layer_flags = 0;
    TC0100SCN_Ctrl->bg0_y = 8;
    TC0100SCN_Ctrl->bg0_x = 17;

    set_colors(0, sizeof(finalb_palette) / 2, finalb_palette);

    *TC011PCR_WHAT = 0;

    enable_interrupts();

    uint8_t write_idx = 0;

    memcpy(TC0100SCN->fg0_gfx + ( 0x20 * 8 ), _binary_src_font_chr_start, _binary_src_font_chr_end - _binary_src_font_chr_start);

    /*TC0100SCN->fg0_gfx[8]  = 0b00000000'11111111;*/
    /*TC0100SCN->fg0_gfx[9]  = 0b11111111'00000000;*/
    /*TC0100SCN->fg0_gfx[10] = 0b11111111'11111111;*/
    /*TC0100SCN->fg0_gfx[11] = 0b10000110'00000101;*/
    /*TC0100SCN->fg0_gfx[12] = 0b10000110'00000101;*/
    /*TC0100SCN->fg0_gfx[13] = 0b10000110'00000101;*/
    /*TC0100SCN->fg0_gfx[14] = 0b10000110'00000101;*/
    /*TC0100SCN->fg0_gfx[15] = 0b00000000'11111111;*/
    /**/
    while(1)
    {
        on_layer(BG0); pen_color(0);
        sym_at(0, 0, 1);
        sym_at(0, 27, 1);
        sym_at(39, 0, 1);
        sym_at(39, 27, 1);

        on_layer(BG0);
        pen_color(0);
        move_to(2, 2);
        print("VBL: %05X  DMA: %05X", vblank_count, dma_count);

        pen_color(6);
        print_at(2, 4, "LAYER BG0");
        on_layer(BG1);
        pen_color(3);
        print_at(2, 5, "LAYER BG1");

        on_layer(FG0);
        pen_color(0);
        print_at(2, 8, "The quick brown fox\njumps over the lazy dog.\n0123456789?/=-+*");
    }

    return 0;
}

