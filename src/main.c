#include <stdint.h>
#include <stdbool.h>
#include "printf/printf.h"

#include "util.h"
#include "interrupts.h"
#include "tilemap.h"
#include "input.h"
#include "system.h"

#include "palette.h"

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

void wait_vblank()
{
    uint32_t current = vblank_count;
    while( current == vblank_count )
    {
    }
}

void wait_dma()
{
    uint32_t current = dma_count;
    while( current == dma_count )
    {
    }
}


uint8_t sine_wave[256] =
{
    0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96,
    0x99, 0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE,
    0xB1, 0xB3, 0xB6, 0xB9, 0xBC, 0xBF, 0xC1, 0xC4,
    0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8,
    0xDA, 0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8,
    0xEA, 0xEB, 0xED, 0xEF, 0xF0, 0xF1, 0xF3, 0xF4,
    0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC,
    0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
    0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6,
    0xF5, 0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB,
    0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
    0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9,
    0xC7, 0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3,
    0xB1, 0xAE, 0xAB, 0xA8, 0xA5, 0xA2, 0x9F, 0x9C,
    0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83,
    0x80, 0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A,
    0x67, 0x64, 0x61, 0x5E, 0x5B, 0x58, 0x55, 0x52,
    0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C,
    0x39, 0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28,
    0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
    0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C,
    0x0B, 0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04,
    0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03,
    0x03, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x11, 0x13, 0x15,
    0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24,
    0x26, 0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37,
    0x39, 0x3C, 0x3F, 0x41, 0x44, 0x47, 0x4A, 0x4D,
    0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64,
    0x67, 0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D
};

extern char _binary_src_font_chr_start[];
extern char _binary_src_font_chr_end[];

#define NUM_SCREENS 3

static uint32_t frame_count;

void init_scn_general()
{
    memset(TC0100SCN, 0, sizeof(TC0100SCN_Layout));

    TC0100SCN_Ctrl->bg1_y = 0;
    TC0100SCN_Ctrl->bg1_x = 9;
    TC0100SCN_Ctrl->fg0_y = 0;
    TC0100SCN_Ctrl->fg0_x = 9;
    TC0100SCN_Ctrl->system_flags = 0;
    TC0100SCN_Ctrl->layer_flags = 0;
    TC0100SCN_Ctrl->bg0_y = 0;
    TC0100SCN_Ctrl->bg0_x = 9;
    memcpy(TC0100SCN->fg0_gfx + ( 0x20 * 8 ), _binary_src_font_chr_start, _binary_src_font_chr_end - _binary_src_font_chr_start);

    set_colors(0, sizeof(finalb_palette) / 2, finalb_palette);

    *TC011PCR_WHAT = 0;

    frame_count = 0;
}

void update_scn_general()
{
    // Max extent corner boundaries
    on_layer(BG0); pen_color(0);
    sym_at(1, 1, 1);
    sym_at(1, 28, 1);
    sym_at(40, 1, 1);
    sym_at(40, 28, 1);

    // Should not be visible
    pen_color(1);
    sym_at(0, 1, 0x1b);
    sym_at(1, 0, 0x1b);
    sym_at(0, 28, 0x1b);
    sym_at(1, 29, 0x1b);
    sym_at(40, 0, 0x1b);
    sym_at(41, 1, 0x1b);
    sym_at(40, 29, 0x1b);
    sym_at(41, 28, 0x1b);

    on_layer(BG0);
    pen_color(0);
    move_to(3, 3);
    print("VBL: %05X  FRAME: %05X", vblank_count, frame_count);

    pen_color(6);
    print_at(4, 5, "LAYER BG0");
    on_layer(BG1);
    pen_color(3);
    print_at(4, 6, "LAYER BG1");

    on_layer(FG0);
    pen_color(0);
    print_at(4, 9, "The quick brown fox\njumps over the lazy dog.\n0123456789?/=-+*");


    on_layer(BG0);
    pen_color(9);
    print_at(10, 14, "ROW\nSCROLL\nBG0");
    for( int y = 0; y < 24; y++ )
    {
        TC0100SCN->bg0_rowscroll[14 * 8 + y] = sine_wave[(frame_count*2+(y*4)) & 0xff] >> 4;
    }

    on_layer(BG1);
    pen_color(2);
    print_at(20, 14, "COL\nSCROLL\nBG1\n\n\nROWCOL\nSCROLL");
    for( int x = 0; x < 6; x++ )
    {
        TC0100SCN->bg1_colscroll[20 + x] = sine_wave[(frame_count*2+(x*8)) & 0xff] >> 4;
    }
    for( int y = 0; y < 16; y++ )
    {
        TC0100SCN->bg1_rowscroll[20 * 8 + y] = sine_wave[(frame_count*2+(y*4)) & 0xff] >> 4;
    }

    frame_count++;
}

uint16_t system_flags, layer_flags;

void init_scn_control_access()
{
    memset(TC0100SCN, 0, sizeof(TC0100SCN_Layout));

    TC0100SCN_Ctrl->bg1_y = 0;
    TC0100SCN_Ctrl->bg1_x = 9;
    TC0100SCN_Ctrl->fg0_y = 0;
    TC0100SCN_Ctrl->fg0_x = 9;
    TC0100SCN_Ctrl->system_flags = 0;
    TC0100SCN_Ctrl->layer_flags = 0;
    TC0100SCN_Ctrl->bg0_y = 0;
    TC0100SCN_Ctrl->bg0_x = 9;
    memcpy(TC0100SCN->fg0_gfx + ( 0x20 * 8 ), _binary_src_font_chr_start, _binary_src_font_chr_end - _binary_src_font_chr_start);

    set_colors(0, sizeof(finalb_palette) / 2, finalb_palette);

    *TC011PCR_WHAT = 0;

    frame_count = 0;
    system_flags = 0;
    layer_flags = 0;

    const char *msg1 = "UP/DOWN ADJUST SYSTEM FLAGS";
    const char *msg2 = "LEFT/RIGHT ADJUST LAYER FLAGS";

    pen_color(0);
    on_layer(BG0);
    print_at(4, 20, msg1);
    on_layer(BG1);
    print_at(4, 20, msg1);
    on_layer(FG0);
    print_at(4, 20, msg1);

    on_layer(BG0);
    print_at(4, 21, msg2);
    on_layer(BG1);
    print_at(4, 21, msg2);
    on_layer(FG0);
    print_at(4, 21, msg2);
}

void update_scn_control_access()
{
    bool changed = false;

    if(input_pressed(LEFT))
    {
        if (system_flags == 0)
            system_flags = 1;
        else
            system_flags = system_flags << 1;
        changed = true;
    }

    if(input_pressed(RIGHT))
    {
        if (system_flags == 0)
            system_flags = 1 << 15;
        else
            system_flags = system_flags >> 1;
        changed = true;
    }

    if(input_pressed(UP))
    {
        if (layer_flags == 0)
            layer_flags = 1;
        else
            layer_flags = layer_flags << 1;
        changed = true;
    }

    if(input_pressed(DOWN))
    {
        if (layer_flags == 0)
            layer_flags = 1 << 15;
        else
            layer_flags = layer_flags >> 1;
        changed = true;
    }

    TC0100SCN_Ctrl->system_flags = system_flags;
    TC0100SCN_Ctrl->layer_flags = layer_flags;

    if (changed || frame_count == 0)
    {
        pen_color(0);
        on_layer(BG0);
        print_at(4, 4, "LAYER: %06X", layer_flags);
        on_layer(BG1);
        print_at(4, 5, "LAYER: %06X", layer_flags);
        on_layer(FG0);
        print_at(4, 6, "LAYER: %06X", layer_flags);

        pen_color(0);
        on_layer(BG0);
        print_at(20, 4, "SYSTEM: %06X", system_flags);
        on_layer(BG1);
        print_at(20, 5, "SYSTEM: %06X", system_flags);
        on_layer(FG0);
        print_at(20, 6, "SYSTEM: %06X", system_flags);
    }

    frame_count++;
}

void init_obj_general()
{
    memset(TC0100SCN, 0, sizeof(TC0100SCN_Layout));

    TC0100SCN_Ctrl->bg1_y = 0;
    TC0100SCN_Ctrl->bg1_x = 9;
    TC0100SCN_Ctrl->fg0_y = 0;
    TC0100SCN_Ctrl->fg0_x = 9;
    TC0100SCN_Ctrl->system_flags = 0;
    TC0100SCN_Ctrl->layer_flags = 0;
    TC0100SCN_Ctrl->bg0_y = 0;
    TC0100SCN_Ctrl->bg0_x = 9;
    memcpy(TC0100SCN->fg0_gfx + ( 0x20 * 8 ), _binary_src_font_chr_start, _binary_src_font_chr_end - _binary_src_font_chr_start);

    set_colors(0, sizeof(finalb_palette) / 2, finalb_palette);
    *TC011PCR_WHAT = 0;

    frame_count = 0;
}

uint16_t obj_test_data[] =
{
    0x0000, 0x0000, 0x0000, 0x8000, 0x0000, 0x0300, 0x0000, 0x0000,
    0x0000, 0x0000, 0xA000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x5080, 0x00A4, 0x090C, 0x0000, 0x0000, 0x0000,
    0x13D3, 0x0000, 0x0000, 0x0000, 0x3D00, 0x0000, 0x0000, 0x0000,
    0x13D7, 0x0000, 0x0000, 0x0000, 0x3D00, 0x0000, 0x0000, 0x0000,
    0x13CF, 0x0000, 0x0000, 0x0000, 0xCD00, 0x0000, 0x0000, 0x0000,
    0x13D2, 0x0000, 0x0000, 0x0000, 0x7D00, 0x0000, 0x0000, 0x0000,
    0x13D6, 0x0000, 0x0000, 0x0000, 0x7D00, 0x0000, 0x0000, 0x0000,
    0x13CE, 0x0000, 0x0000, 0x0000, 0xCD00, 0x0000, 0x0000, 0x0000,
    0x13D1, 0x0000, 0x0000, 0x0000, 0x7D00, 0x0000, 0x0000, 0x0000,
    0x13D5, 0x0000, 0x0000, 0x0000, 0x7D00, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xCD00, 0x0000, 0x0000, 0x0000,
    0x13D0, 0x0000, 0x0000, 0x0000, 0x7D00, 0x0000, 0x0000, 0x0000,
    0x13D4, 0x0000, 0x0000, 0x0000, 0x7500, 0x0000, 0x0000, 0x0000,
};

void update_obj_general()
{
    wait_dma();
    memcpyw(TC0200OBJ, obj_test_data, sizeof(obj_test_data) / 2);
    wait_dma();

    on_layer(BG0);
    pen_color(1);
    move_to(5, 4);
    for( int x = 0; x < 15; x++ )
    {
        print("%04X  %04X\n", TC0200OBJ[x].c, TC0200OBJ[x].e);
    }

    frame_count++;
}


void init_screen(int screen)
{
    switch(screen)
    {
        case 0: init_scn_general(); break;
        case 1: init_scn_control_access(); break;
        case 2: init_obj_general(); break;
        default: break;
    }
}

void update_screen(int screen)
{
    switch(screen)
    {
        case 0: update_scn_general(); break;
        case 1: update_scn_control_access(); break;
        case 2: update_obj_general(); break;
        default: break;
    }
}

void deinit_screen(int screen)
{
    switch(screen)
    {
        default: break;
    }
}

int main(int argc, char *argv[])
{
    memsetw(TC0200OBJ, 0, 0x8000);

    memset(TC0100SCN, 0, sizeof(TC0100SCN_Layout));

    enable_interrupts();

    uint32_t system_flags = 0;

    int current_screen = 2;

    init_screen(current_screen);
    
    while(1)
    {
        input_update();

        if (input_pressed(START))
        {
            deinit_screen(current_screen);
            current_screen = ( current_screen + 1 ) % NUM_SCREENS;
            init_screen(current_screen);
        }

        update_screen(current_screen);
    }

    return 0;
}


