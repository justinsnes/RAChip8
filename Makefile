CC = gcc
CFLAGS = -c
OUTPUTFLAGS = -lSDL2 -g3 -O0 # for debugging
RM = rm -f

all: RAChip8

RAChip8: RAChip8.o Chip8.o Display.o Opcodes.o
	$(CC) RAChip8.o Chip8.o Display.o Opcodes.o $(OUTPUTFLAGS) -o RAChip8
	chmod +x RAChip8

RAChip8.o: RAChip8.c
	$(CC) $(CFLAGS) RAChip8.c $(OUTPUTFLAGS)

Chip8.o: Chip8.c Chip8.h
	$(CC) $(CFLAGS) Chip8.c $(OUTPUTFLAGS)

Display.o: Display.c Display.h
	$(CC) $(CFLAGS) Display.c $(OUTPUTFLAGS)

Opcodes.o: Opcodes.c Opcodes.h
	$(CC) $(CFLAGS) Opcodes.c $(OUTPUTFLAGS)

target: dependencies
	action

clean: 
	rm *.o