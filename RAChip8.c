#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//sudo apt-get install libsdl2-dev
#include <SDL2/SDL.h>

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

void initialize(Chip8 *chip8) {
    // 0x000 to 0x1FF reserved for interpreter itself
    chip8->pc = 0x200; // Program counter starts at 0x200
    chip8->opcode = 0;
    chip8->I = 0;
    chip8->sp = 0;

    // Clear stack, registers, and memory
    for (int i = 0; i < STACK_SIZE; ++i) {
        chip8->stack[i] = 0;
    }
    for (int i = 0; i < GENERAL_REGISTER_COUNT; ++i) {
        chip8->V[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        chip8->memory[i] = 0;
    }

    // Load fontset
    // Example translation of D character font to binary:
    // 0xE0 = 11100000
    // 0x90 = 10010000
    // 0x90 = 10010000
    // 0x90 = 10010000
    // 0xE0 = 11100000
    uint8_t chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int i = 0; i < 80; ++i) {
        chip8->memory[i] = chip8_fontset[i];
    }

    // Reset timers
    chip8->delay_timer = 0;
    chip8->sound_timer = 0;
}

// 1nnn - JP addr
void opcode_1nnn(Chip8 *chip8) {
    // The interpreter sets the program counter to nnn.
    uint16_t nnn = chip8->opcode & 0x0FFF;
    chip8->pc = nnn;
}

// 6xnn - LD Vx, byte
void opcode_6xnn(Chip8 *chip8) {
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t nn = chip8->opcode & 0x00FF;
    chip8->V[x] = nn;

    chip8->pc += 2;
}

// Annn - LD I, addr
void opcode_Annn(Chip8 *chip8) {
    chip8->I = chip8->opcode & 0x0FFF;

    chip8->pc += 2;
}

// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void opcode_Dxyn(Chip8 *chip8) {
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t y = (chip8->opcode & 0x00F0) >> 4;
    uint8_t nBytes = chip8->opcode & 0x000F;

    // each byte represents a row of 8 pixels aka 1 yline
    chip8->V[0xF] = 0;
    for (int yline = 0; yline < nBytes; ++yline) {
        uint8_t pixel = chip8->memory[chip8->I + yline];
        for (int xline = 0; xline < 8; ++xline) {
            // The interpreter reads n bytes from memory, starting at the address stored in I.
            // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
            // Sprites are XORed onto the existing screen.
            // If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
            // If the sprite is positioned so part of it is outside the coordinates of the display,
            // it wraps around to the opposite side of the screen.
            if ((pixel & (0x80 >> xline)) != 0) {
                setPixel(&chip8->display, chip8->V[x] + xline, chip8->V[y] + yline, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
                // if (chip8->display[(chip8->V[x] + xline + ((chip8->V[y] + yline) * DISPLAY_WIDTH))] == 1) {
                //     chip8->V[0xF] = 1;
                // }
                // chip8->display[chip8->V[x] + xline + ((chip8->V[y] + yline) * DISPLAY_WIDTH)] ^= 1;
            }
            
        }
    }
    updateDisplay(&chip8->display);

    chip8->pc += 2;
}

int main(int argc, char **argv) {
    Chip8 chip8;
    initialize(&chip8);

    // Load ROM into memory starting at 0x200
    FILE *rom = fopen("TestROMs/chiptest-mini-offstatic.ch8", "rb");
    if (rom == NULL) {
        fprintf(stderr, "Failed to open ROM\n");
        return 1;
    }
    fread(chip8.memory + 0x200, 1, MEMORY_SIZE - 0x200, rom);
    fclose(rom);

    // Print memory at address 0x200
    printf("Memory at 0x200: %02X%02X\n", chip8.memory[0x200], chip8.memory[0x201]);
    printf("Memory at 0x202: %02X%02X\n", chip8.memory[0x202], chip8.memory[0x203]);
    printf("Memory at 0x204: %02X%02X\n", chip8.memory[0x204], chip8.memory[0x205]);
    printf("Memory at 0x21E: %02X%02X\n", chip8.memory[0x21E], chip8.memory[0x21F]);
    printf("Memory at 0x220: %02X%02X\n", chip8.memory[0x220], chip8.memory[0x221]);

    // Display.c implementation
    //Display display;
    initDisplay(&chip8.display, "CHIP-8 Emulator", DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10);

    // Set a few pixels in the corners for testing
    setPixel(&chip8.display, 0, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, DISPLAY_WIDTH - 1, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, 0, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    updateDisplay(&chip8.display);

    int breakpointnum = 2 + 2;

    // Main emulation loop
    for (;;) {
        
        // Fetch, decode, and execute instructions
        // Fetch opcode using bitwise or operator. 
        // First byte is the high byte. second byte is the low byte.
        chip8.opcode = chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1];

        // Decode and execute opcode
        switch (chip8.opcode & 0xF000) {
            case 0x1000:
                opcode_1nnn(&chip8);
                break;
            case 0x6000:
                opcode_6xnn(&chip8);
                break;
            case 0xA000:
                opcode_Annn(&chip8);
                break;
            case 0xD000:
                opcode_Dxyn(&chip8);
                break;
            default:
                fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                break;
        }

        // Update timers
        if (chip8.delay_timer > 0) {
            --chip8.delay_timer;
        }
        if (chip8.sound_timer > 0) {
            --chip8.sound_timer;
        }
    }

    return 0;
}