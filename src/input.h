#if !defined(INPUT_H)
#define INPUT_H 1

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    UP    = 0x0001,
    DOWN  = 0x0002,
    LEFT  = 0x0004,
    RIGHT = 0x0008,
    BTN1  = 0x0010,
    BTN2  = 0x0020,
    BTN3  = 0x0040,
    START = 0x0080,
} InputKey;

void input_update();
uint16_t input_state();
bool input_down(InputKey key);
bool input_released(InputKey key);
bool input_pressed(InputKey key);

#endif // !defined(INPUT_H)
