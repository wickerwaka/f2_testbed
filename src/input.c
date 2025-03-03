#include "input.h"

#include "tc0220ioc.h"

static uint16_t s_prev = 0;
static uint16_t s_cur = 0;

TC0220IOC_Control *IOC = (TC0220IOC_Control *)0x300000;

void input_update()
{
    s_prev = s_cur;
    s_cur = IOC->p1;
}

uint16_t input_state()
{
    return s_cur;
}

bool input_down(InputKey key)
{
    return (s_cur & key) != key;
}

bool input_released(InputKey key)
{
    return ((s_cur & key) != 0) && (((s_prev ^ s_cur) & key) != 0);
}

bool input_pressed(InputKey key)
{
    return ((s_cur & key) != key) && (((s_prev ^ s_cur) & key) != 0);
}
