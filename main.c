#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include <Windows.h>
#include "chip8.h"

#define PIXEL_SCALE 15

uint8_t running = 1;

uint8_t keymap[] = {
    SDL_SCANCODE_X,
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D,
    SDL_SCANCODE_Z, SDL_SCANCODE_C, SDL_SCANCODE_4,
    SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V
};

int main(int argc, char* argv[])
{
    struct Chip8 chip8;
    load_rom(&chip8, argv[1]);

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, PIXEL_SCALE * 64, PIXEL_SCALE * 32, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        printf("Error creating the window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        printf("Error creating the renderer: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Event event;

    LARGE_INTEGER frequency;
    LARGE_INTEGER t1, t2;
    double elapsed_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&t1);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    for (int i = 0; i <= 0xF; i++)
                    {
                        if (keymap[i] == event.key.keysym.scancode)
                            chip8.key[i] = 1;
                    }
                    
                    break;

                case SDL_KEYUP:
                    for (int i = 0; i <= 0xF; i++)
                    {
                        if (keymap[i] == event.key.keysym.scancode)
                            chip8.key[i] = 0;
                    }

                    break;
            }
        }

        QueryPerformanceCounter(&t2);
        elapsed_time = (t2.QuadPart - t1.QuadPart) * 1000000.0 / frequency.QuadPart;

        if (elapsed_time > CHIP8_FREQ)
        {
            emulate_cycle(&chip8);
            QueryPerformanceCounter(&t1);
        }

        if (chip8.draw)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_Rect pixel;
            pixel.h = PIXEL_SCALE;
            pixel.w = PIXEL_SCALE;

            for (int x = 0; x < 64; x++)
            {
                pixel.x = x * PIXEL_SCALE;

                for (int y = 0; y < 32; y++)
                {
                    pixel.y = y * PIXEL_SCALE;

                    if (chip8.graphics[x + y * 64])
                        SDL_RenderFillRect(renderer, &pixel);
                }
            }

            SDL_RenderPresent(renderer);

            chip8.draw = 0;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
