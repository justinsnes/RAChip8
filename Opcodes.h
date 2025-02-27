#ifndef OPCODES_H
#define OPCODES_H

void opcode_00E0(Chip8 *chip8);
void opcode_00EE(Chip8 *chip8);
void opcode_1nnn(Chip8 *chip8);
void opcode_2nnn(Chip8 *chip8);
void opcode_3xkk(Chip8 *chip8);
void opcode_4xkk(Chip8 *chip8);
void opcode_5xy0(Chip8 *chip8);
void opcode_6xnn(Chip8 *chip8);
void opcode_7xkk(Chip8 *chip8);
void opcode_9xy0(Chip8 *chip8);
void opcode_Annn(Chip8 *chip8);
void opcode_Bnnn(Chip8 *chip8);
void opcode_Cxkk(Chip8 *chip8);
void opcode_Dxyn(Chip8 *chip8);

#endif // OPCODES_H