#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    int width;
    int height;
    u_int8_t pixels[DISPLAY_WIDTH][DISPLAY_HEIGHT];
} Display;

// Function to initialize the display
int initDisplay(Display *display, const char *title, int width, int height);

// Function to clear the display
void clearDisplay(Display *display);

// Function to update the display
void updateDisplay(Display *display);

// Function to set a pixel on the display
void setPixel(Display *display, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Function to destroy the display. Frees up the memory allocated to the renderer and window
void destroyDisplay(Display *display);

#endif // DISPLAY_H