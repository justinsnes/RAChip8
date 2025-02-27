#include "Chip8.h"
#include "Opcodes.h"

// 00E0 - CLS
void opcode_00E0(Chip8 *chip8) {
    // Clear the display.
    clearDisplay(&chip8->display);

    chip8->pc += 2;
}

// 00EE - RET
void opcode_00EE(Chip8 *chip8) {
    // The interpreter sets the program counter to the address at the top of the stack, 
    // then subtracts 1 from the stack pointer.
    chip8->pc = chip8->stack[chip8->sp];
    chip8->sp--;

    chip8->pc += 2;
}

// 1nnn - JP addr
void opcode_1nnn(Chip8 *chip8) {
    // The interpreter sets the program counter to nnn.
    uint16_t nnn = chip8->opcode & 0x0FFF;
    chip8->pc = nnn;
}

// 2nnn - CALL addr
void opcode_2nnn(Chip8 *chip8) {
    // The interpreter increments the stack pointer, 
    // then puts the current PC on the top of the stack. 
    // The PC is then set to nnn.
    chip8->sp++;
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->pc = chip8->opcode & 0x0FFF;
}

// 3xkk - SE Vx, byte
// (SE =  Set if Equal)
void opcode_3xkk(Chip8 *chip8) {
    // Interpreter compares register V[x] to kk. 
    // If equal, increment the Program Counter by an additional 2 bytes. 
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t kk = chip8->opcode & 0x00FF;
    if (chip8->V[x] == kk) {
        chip8->pc += 2;
    }

    chip8->pc += 2;
}

// 4xkk - SNE Vx, byte
// (SNE = Skip if Not Equal)
void opcode_4xkk(Chip8 *chip8) {
    // Interpreter compares register V[x] to kk. 
    // If not equal, increment the Program Counter by an additional 2 bytes. 
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t kk = chip8->opcode & 0x00FF;
    if (chip8->V[x] != kk) {
        chip8->pc += 2;
    }

    chip8->pc += 2;
}

// 5xy0 - SE Vx, Vy
void opcode_5xy0(Chip8 *chip8) {
    // The interpreter compares register Vx to register Vy. 
    // If they are equal, increment the program counter by 2.
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t y = (chip8->opcode & 0x00F0) >> 4;
    if (chip8->V[x] == chip8->V[y]) {
        chip8->pc += 2;
    }

    chip8->pc += 2;
}

// 6xnn - LD Vx, byte
void opcode_6xnn(Chip8 *chip8) {
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t nn = chip8->opcode & 0x00FF;
    chip8->V[x] = nn;

    chip8->pc += 2;
}

// 7xkk - ADD Vx, byte
void opcode_7xkk(Chip8 *chip8) {
    // Adds the value kk to the value of register Vx, then stores the result in Vx.
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t kk = chip8->opcode & 0x00FF;
    chip8->V[x] += kk;

    chip8->pc += 2;
}

// 9xy0 - SNE Vx, Vy
void opcode_9xy0(Chip8 *chip8) {
    // The interpreter compares register Vx to register Vy. 
    // If they are not equal, increment the program counter by 2.
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t y = (chip8->opcode & 0x00F0) >> 4;
    if (chip8->V[x] != chip8->V[y]) {
        chip8->pc += 2;
    }

    chip8->pc += 2;
}

// Annn - LD I, addr
void opcode_Annn(Chip8 *chip8) {
    chip8->I = chip8->opcode & 0x0FFF;

    chip8->pc += 2;
}

// Bnnn - JP V0, addr
void opcode_Bnnn(Chip8 *chip8) {
    // The program counter is set to nnn plus the value of V0.
    uint16_t nnn = chip8->opcode & 0x0FFF;
    chip8->pc = nnn + chip8->V[0x0];
}

// Cxkk - RND Vx, byte
void opcode_Cxkk(Chip8 *chip8) {
    // The interpreter generates a random number from 0 to 255, 
    // which is then ANDed with the value kk. The results are stored in Vx.
    uint8_t rng = (uint8_t)((double)rand() / ((double)RAND_MAX + 1) * 255);
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t kk = chip8->opcode & 0x00FF;
    chip8->V[x] = rng & kk;

    chip8->pc += 2;
}

// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at screen coordinates (Vx, Vy)
// These sprites are XORed onto the existing screen. 
// Sprites can be up to 15 bytes making it a max size of 15x8 pixels.
// If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
// If the sprite is positioned so part of it is outside the coordinates of the display,
// it wraps around to the opposite side of the screen.
void opcode_Dxyn(Chip8 *chip8) {
    uint8_t x = (chip8->opcode & 0x0F00) >> 8;
    uint8_t y = (chip8->opcode & 0x00F0) >> 4;
    uint8_t nBytes = chip8->opcode & 0x000F;

    // each byte represents a row of 8 pixels aka 1 yline
    chip8->V[0xF] = 0;
    for (int yline = 0; yline < nBytes; ++yline) {
        uint8_t spriteByte = chip8->memory[chip8->I + yline];
        for (int xline = 0; xline < 8; ++xline) {
            // 0x80 is 10000000. This is our starting bit mask to test. 
            // each bit in the Sprite Byte will be tested.
            if ((spriteByte & (0x80 >> xline)) != 0) {
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