#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

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
    FILE *rom = fopen("TestROMs/Breakout [Carmelo Cortez, 1979].ch8", "rb");
    //FILE *rom = fopen("TestROMs/Pong (1 player).ch8", "rb");
    //FILE *rom = fopen("TestROMs/Hi-Lo [Jef Winsor, 1978].ch8", "rb");
    //FILE *rom = fopen("TestROMs/chiptest-offstatic.ch8", "rb");
    if (rom == NULL) {
        fprintf(stderr, "Failed to open ROM\n");
        return 1;
    }
    fread(chip8.memory + 0x200, 1, MEMORY_SIZE - 0x200, rom);
    fclose(rom);

    // Print memory at address 0x200
    printf("Memory at 0x200: %02X%02X\n", chip8.memory[0x200], chip8.memory[0x201]);

    initDisplay(&chip8.display, "CHIP-8 Emulator", DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10);

    // Set a few pixels in the corners for testing
    setPixel(&chip8.display, 0, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, DISPLAY_WIDTH - 1, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, 0, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    setPixel(&chip8.display, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    updateDisplay(&chip8.display);

    // for measuring time to obtain 60hz/60fps
    Uint32 lastTime = SDL_GetTicks();
    double delta = 0;

    // Main emulation loop
    for (;;) {
        // Helps to slow down the emulation loop for a game like pong but 
        // breaks offstatic's chiptest.
        usleep(1000 * 10); // sleep for 10ms (in an effort to put a cap on CPU cycles)
        // a truely accurate implementation would be to measure each instruction's
        // microseconds individually. 

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
            case 0x8000:
                switch (chip8.opcode & 0x000F) {
                    case 0x0000:
                        opcode_8xy0(&chip8);
                        break;
                    case 0x0001:
                        opcode_8xy1(&chip8);
                        break;
                    case 0x0002:
                        opcode_8xy2(&chip8);
                        break;
                    case 0x0003:
                        opcode_8xy3(&chip8);
                        break;
                    case 0x0004:
                        opcode_8xy4(&chip8);
                        break;
                    case 0x0005:
                        opcode_8xy5(&chip8);
                        break;
                    case 0x0006:
                        opcode_8xy6(&chip8);
                        break;
                    case 0x0007:
                        opcode_8xy7(&chip8);
                        break;
                    case 0x000E:
                        opcode_8xyE(&chip8);
                        break;
                    default:
                        fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                        break;
                }
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
            case 0xE000:
                switch (chip8.opcode & 0x00FF) {
                    case 0x009E:
                        opcode_Ex9E(&chip8);
                        break;
                    case 0x00A1:
                        opcode_ExA1(&chip8);
                        break;
                    default:
                        fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                        break;
                }
                break;
            case 0xF000:
                switch (chip8.opcode & 0x00FF) {
                    case 0x0007:
                        opcode_Fx07(&chip8);
                        break;
                    case 0x000A:
                        opcode_Fx0A(&chip8);
                        break;
                    case 0x0015:
                        opcode_Fx15(&chip8);
                        break;
                    case 0x0018:
                        opcode_Fx18(&chip8);
                        break;
                    case 0x001E:
                        opcode_Fx1E(&chip8);
                        break;
                    case 0x0029:
                        opcode_Fx29(&chip8);
                        break;
                    case 0x0033:
                        opcode_Fx33(&chip8);
                        break;
                    case 0x0055:
                        opcode_Fx55(&chip8);
                        break;
                    case 0x0065:
                        opcode_Fx65(&chip8);
                        break;
                    default:
                        fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                        break;
                }
                break;
            default:
                fprintf(stderr, "Unknown opcode: %04X\n", chip8.opcode);
                break;
        }

        // Update timers at a rate of 60Hz
        Uint32 currentTime = SDL_GetTicks();
        delta = (currentTime - lastTime);
        bool nextFrame = false;
        if (delta >= 1000.0 / 60.0) {
            fprintf(stderr, "frame detected at %d - %d = %f\n", currentTime, lastTime, delta);
            lastTime = currentTime;
            delta = 0;
            nextFrame = true;
        }

        if (nextFrame) {
            if (chip8.delay_timer > 0) {
                --chip8.delay_timer;
            }
            if (chip8.sound_timer > 0) {
                --chip8.sound_timer;
            }
        }
        
    }

    return 0;
}