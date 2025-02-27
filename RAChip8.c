#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//sudo apt-get install libsdl2-dev
#include <SDL2/SDL.h>

#include "Chip8.h"
#include "Display.h"
#include "Opcodes.h"

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

int main(int argc, char **argv) {
    Chip8 chip8;
    initialize(&chip8);

    // Load ROM into memory starting at 0x200
    FILE *rom = fopen("TestROMs/chiptest-offstatic.ch8", "rb");
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
            case 0x0000:
                switch (chip8.opcode & 0x00FF) {
                    case 0x00E0:
                        opcode_00E0(&chip8);
                        break;
                    case 0x00EE:
                        opcode_00EE(&chip8);
                        break;
                    default:
                        fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                        break;
                }
                break;
            case 0x1000:
                opcode_1nnn(&chip8);
                break;
            case 0x2000:
                opcode_2nnn(&chip8);
                break;
            case 0x3000:
                opcode_3xkk(&chip8);
                break;
            case 0x4000:
                opcode_4xkk(&chip8);
                break;
            case 0x5000:
                opcode_5xy0(&chip8);
                break;
            case 0x6000:
                opcode_6xnn(&chip8);
                break;
            case 0x7000:
                opcode_7xkk(&chip8);
                break;
            case 0x9000:
                opcode_9xy0(&chip8);
                break;
            case 0xA000:
                opcode_Annn(&chip8);
                break;
            case 0xB000:
                opcode_Bnnn(&chip8);
                break;
            case 0xC000:
                opcode_Cxkk(&chip8);
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