#ifndef KEYPAD_H
#define KEYPAD_H

#include <SDL2/SDL.h>

#define KEYS 16
extern uint8_t Keypad[KEYS];

int checkForKeyPress(SDL_Event *event);

#endif // KEYPAD_H