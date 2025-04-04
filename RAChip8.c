#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
// sound
#include <math.h>

//sudo apt-get install libsdl2-dev
#include <SDL2/SDL.h>

#include "Chip8.h"
#include "Display.h"
#include "Opcodes.h"
#include "Keypad.h"

void my_audio_callback(void* userdata, Uint8* stream, int length)
{
    Sint16* buffer = (Sint16*) stream;
    int sample_count = length / 2;

    for (int i = 0; i < sample_count; ++i)
    {
        //buffer[i] = rand() % 30000; // white noise
        //buffer[i] = 4000 * cos(2 * 3.14 * 440 * i / 44100); // sine wave
        buffer[i] = 2000 * (i % 128 < 64 ? 1 : -1); // Simple square wave
        // amplitude * (i % period < period / 2 ? 1 : -1)
    }
}

int main(int argc, char **argv) {
    Chip8 chip8;
    initializeChip8(&chip8);

    // reset pressed key states
    for (int i = 0; i < KEYS; i++) {
        pressedKeys[i] = 0;
    }

    // generate seed for randomly generated numbers 
    // (rand() would always be the same otherwise)
    srand(time(NULL));

    // Load ROM into memory starting at 0x200
    //FILE *rom = fopen("TestROMs/Breakout [Carmelo Cortez, 1979].ch8", "rb");
    FILE *rom = fopen("TestROMs/Pong (1 player).ch8", "rb");
    //FILE *rom = fopen("TestROMs/Hi-Lo [Jef Winsor, 1978].ch8", "rb");
    //FILE *rom = fopen("TestROMs/chiptest-offstatic.ch8", "rb");
    if (rom == NULL) {
        fprintf(stderr, "Failed to open ROM\n");
        return 1;
    }
    fread(chip8.memory + 0x200, 1, MEMORY_SIZE - 0x200, rom);
    fclose(rom);

    // Print memory at address 0x200
    //printf("Memory at 0x200: %02X%02X\n", chip8.memory[0x200], chip8.memory[0x201]);

    initDisplay(&chip8.display, "CHIP-8 Emulator", DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10);

    // Set a few pixels in the corners for testing
    // setPixel(&chip8.display, 0, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    // setPixel(&chip8.display, DISPLAY_WIDTH - 1, 0, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    // setPixel(&chip8.display, 0, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    // setPixel(&chip8.display, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, RED_VAL, GREEN_VAL, BLUE_VAL, ALPHA_VAL);
    // updateDisplay(&chip8.display);

    // for measuring time to obtain 60hz/60fps
    // A close benchmark for attempting to match the Cosmac VIP which
    // Chip8 originally ran on. (roughly 11 frames per second)
    // The true implementation would try to emulate exact timings since they
    // all vary by instruction on the CPU, but that's not needed.
    // The user would know no different.
    Uint32 lastTime = SDL_GetTicks();
    double delta = 0;
    // docs and other resources online recommend this to be at 11
    // but it appears to run best on my machine at 9. especially for games like Breakout
    int instructionsPerFrame = 9;
    int instructionsExecutedThisFrame = 0;

    // Sound
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec spec = {0};
    spec.freq = 14100;
    spec.format = AUDIO_S16SYS;
    spec.channels = 1; // mono. 2 is stereo
    spec.samples = 256;
    spec.callback = my_audio_callback;
    spec.userdata = NULL;
    //SDL_OpenAudio(&spec, NULL);
    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

    SDL_PauseAudioDevice(device_id, 0);
    SDL_Delay(2000); // 2 seconds of audio
    SDL_PauseAudioDevice(device_id, 1);
    SDL_Delay(500);
    bool playingAudio = false;
    // Audio countdown test
    for (int i = 0; i < 3; i++) {
        SDL_PauseAudioDevice(device_id, 0);
        SDL_Delay(500);
        SDL_PauseAudioDevice(device_id, 1);
        SDL_Delay(500);
    }

    // Main emulation loop
    for (;;) {
        // check for user interaction
        // The way SDL_PollEvent works is it invokes SDL_PumpEvents internally
        // then loops through the events in the queue while popping them out.
        // This was why the events weren't being found in other calls when
        // recalling SDL_PollEvent. (Key presses were being registered then dequeued)
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                destroyDisplay(&chip8.display);
                SDL_CloseAudioDevice(device_id);
                SDL_Quit();
                return 0;
            } else if (event.type == SDL_KEYDOWN) {
                for (int i = 0; i < KEYS; i++) {
                    if (event.key.keysym.sym == Keypad[i]) {
                        pressedKeys[i] = i;
                    }
                }
            } else if (event.type == SDL_KEYUP) {
                for (int i = 0; i < KEYS; ++i) {
                    if (event.key.keysym.sym == Keypad[i]) {
                        pressedKeys[i] = 0;
                    }
                }
            }
        }

        // Measure game frames at 60Hz
        Uint32 currentTime = SDL_GetTicks();
        delta = (currentTime - lastTime);
        bool nextFrame = false;
        if (delta >= 1000.0 / 60.0) {
            //fprintf(stderr, "frame detected at %d - %d = %f\n", currentTime, lastTime, delta);
            lastTime = currentTime;
            delta = 0;
            nextFrame = true;
        }

        if (instructionsExecutedThisFrame >= instructionsPerFrame && nextFrame == false) {
            continue;
        }

        // Fetch opcode using bitwise or operator. 
        // First byte is the high byte. second byte is the low byte.
        chip8.opcode = chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1];

        bool isDraw = false;
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
                isDraw = true;
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

        instructionsExecutedThisFrame++;

        if (chip8.sound_timer > 0 && playingAudio == false) {
            playingAudio = true;
            SDL_PauseAudioDevice(device_id, 0); // start audio
        } else if (chip8.sound_timer == 0 && playingAudio == true) {
            playingAudio = false;
            SDL_PauseAudioDevice(device_id, 1); // stop audio
        }

        // Update timers at 60hz
        if (nextFrame) {
            if (chip8.delay_timer > 0) {
                --chip8.delay_timer;
            }
            if (chip8.sound_timer > 0) {
                --chip8.sound_timer;
            }
            instructionsExecutedThisFrame = 0;
        }
        
    }

    return 0;
}