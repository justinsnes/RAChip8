#include "Display.h"

int initDisplay(Display *display, const char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }

    display->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (display->window == NULL) {
        SDL_Quit();
        return -1;
    }

    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL) {
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return -1;
    }

    display->width = width;
    display->height = height;

    // clear window
    clearDisplay(display);

    return 0;
}

void clearDisplay(Display *display) {
    //SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);
    SDL_RenderClear(display->renderer);
    SDL_RenderPresent(display->renderer);
}

void updateDisplay(Display *display) {
    SDL_RenderPresent(display->renderer);
}

void setPixel(Display *display, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Rect rect = {x * 10, y * 10, 10, 10};
    SDL_SetRenderDrawColor(display->renderer, r, g, b, a);
    SDL_RenderFillRect(display->renderer, &rect);
    //SDL_RenderDrawPoint(display->renderer, x, y);
}

void destroyDisplay(Display *display) {
    if (display->renderer != NULL) {
        SDL_DestroyRenderer(display->renderer);
    }
    if (display->window != NULL) {
        SDL_DestroyWindow(display->window);
    }
    SDL_Quit();
}