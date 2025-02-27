#ifndef OPCODES_H
#define OPCODES_H

// 1nnn - JP addr
void opcode_1nnn(Chip8 *chip8);

// 6xnn - LD Vx, byte
void opcode_6xnn(Chip8 *chip8);

// Annn - LD I, addr
void opcode_Annn(Chip8 *chip8);

// Dxyn - DRW Vx, Vy, nibble
void opcode_Dxyn(Chip8 *chip8);

#endif // OPCODES_H