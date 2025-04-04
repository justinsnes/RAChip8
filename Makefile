CC = gcc
CFLAGS = -c
# -lSDL2 for SDL library. -lm for math library. -g3 for debugging. -O0 for no optimization.
OUTPUTFLAGS = -lSDL2 -lm -g3 -O0
RM = rm -f

all: RAChip8
# The OUTPUTFLAGS containing SDL2 and debug flags:
# There was incorrect information before that the OUTPUTFLAGS
# should not be used during object file compilation with -c flag, 
# as these are linker flags that should only be used during the final linking stage.
# This is incorrect, as the OUTPUTFLAGS are REQUIRED during object file compilation
# in order to attach the debugger to the executable using gdb.
RAChip8: RAChip8.o Chip8.o Display.o Keypad.o Opcodes.o
	$(CC) RAChip8.o Chip8.o Display.o Keypad.o Opcodes.o $(OUTPUTFLAGS) -o RAChip8
	chmod +x RAChip8

RAChip8.o: RAChip8.c Chip8.h Opcodes.h Display.h Keypad.h
	$(CC) $(CFLAGS) RAChip8.c $(OUTPUTFLAGS)

Opcodes.o: Opcodes.c Chip8.h Keypad.h
	$(CC) $(CFLAGS) Opcodes.c $(OUTPUTFLAGS)

Chip8.o: Chip8.c Display.h
	$(CC) $(CFLAGS) Chip8.c $(OUTPUTFLAGS)

Display.o: Display.c
	$(CC) $(CFLAGS) Display.c $(OUTPUTFLAGS)

Keypad.o: Keypad.c
	$(CC) $(CFLAGS) Keypad.c $(OUTPUTFLAGS)

target: dependencies
	action

clean: 
	$(RM) *.o
	$(RM) RAChip8
	$(RM) *.gch