#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#include "Display.h"

// Memory addresses are 0x000 to 0xFFF
#define MEMORY_SIZE 4096
// 16 general 8-bit registers. V0 to VF. VF never used by programs 
//  and is used as a flag by some instructions.
// there is also a 16-bit register I. Stores memory addresses.
#define GENERAL_REGISTER_COUNT 16
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define RED_VAL 0
#define GREEN_VAL 255
#define BLUE_VAL 255
#define ALPHA_VAL 255

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    // registers
    uint8_t V[GENERAL_REGISTER_COUNT];
    // register I usually used to store memory addresses
    uint16_t I;
    uint16_t pc;
    uint16_t stack[STACK_SIZE];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;

    //uint8_t keypad[KEYPAD_SIZE];
    //uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    Display display;
    uint16_t opcode;
} Chip8;

#endif // CHIP8_H