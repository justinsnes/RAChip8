#include "Chip8.h"
#include "Opcodes.h"

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