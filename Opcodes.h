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

void opcode_8xy0(Chip8 *chip8);
void opcode_8xy1(Chip8 *chip8);
void opcode_8xy2(Chip8 *chip8);
void opcode_8xy3(Chip8 *chip8);
void opcode_8xy4(Chip8 *chip8);
void opcode_8xy5(Chip8 *chip8);
void opcode_8xy6(Chip8 *chip8);
void opcode_8xy7(Chip8 *chip8);
void opcode_8xyE(Chip8 *chip8);

void opcode_9xy0(Chip8 *chip8);
void opcode_Annn(Chip8 *chip8);
void opcode_Bnnn(Chip8 *chip8);
void opcode_Cxkk(Chip8 *chip8);
void opcode_Dxyn(Chip8 *chip8);

void opcode_Ex9E(Chip8 *chip8);
void opcode_ExA1(Chip8 *chip8);

void opcode_Fx07(Chip8 *chip8);
void opcode_Fx0A(Chip8 *chip8);
void opcode_Fx15(Chip8 *chip8);
void opcode_Fx18(Chip8 *chip8);
void opcode_Fx1E(Chip8 *chip8);
void opcode_Fx29(Chip8 *chip8);
void opcode_Fx33(Chip8 *chip8);
void opcode_Fx55(Chip8 *chip8);
void opcode_Fx65(Chip8 *chip8);

#endif // OPCODES_H