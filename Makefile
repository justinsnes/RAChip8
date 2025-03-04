CC = gcc
CFLAGS = -c
OUTPUTFLAGS = -lSDL2 -g3 -O0 # for debugging
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

RAChip8.o: RAChip8.c
	$(CC) $(CFLAGS) RAChip8.c $(OUTPUTFLAGS)

Opcodes.o: Opcodes.c Opcodes.h 
	$(CC) $(CFLAGS) Opcodes.c $(OUTPUTFLAGS)

Chip8.o: Chip8.c Chip8.h
	$(CC) $(CFLAGS) Chip8.c $(OUTPUTFLAGS)

Display.o: Display.c Display.h
	$(CC) $(CFLAGS) Display.c $(OUTPUTFLAGS)

Keypad.o: Keypad.c Keypad.h
	$(CC) $(CFLAGS) Keypad.c $(OUTPUTFLAGS)

target: dependencies
	action

clean: 
	rm *.o