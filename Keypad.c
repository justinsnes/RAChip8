#include "Keypad.h"

// Keypad layout
// 1 2 3 C = 1 2 3 4
// 4 5 6 D = Q W E R
// 7 8 9 E = A S D F
// A 0 B F = Z X C V
uint8_t Keypad[KEYS] = 
{
    SDLK_x, SDLK_1, SDLK_2, SDLK_3,
    SDLK_q, SDLK_w, SDLK_e, SDLK_a,
    SDLK_s, SDLK_d, SDLK_z, SDLK_c,
    SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

uint8_t pressedKeys[KEYS] = 
{
    0
};

int checkForKeyPress(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        for (int i = 0; i < KEYS; i++)
        {
            if (event->key.keysym.sym == Keypad[i])
            {
                //printf("Key pressed: %x\n", i);
                return i;
            }
        }
    }
    return 255;
}