#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include "chip8.h"

#define WIDTH 640
#define HEIGHT 320

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
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("Error initializing SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

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

    struct Chip8 chip8;
    load_rom(&chip8, argv[1]);

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

        emulate_cycle(&chip8);

        if (chip8.draw)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            SDL_Rect pixel;
            pixel.h = 10;
            pixel.w = 10;

            for (int x = 0; x < 64; x++)
            {
                pixel.x = x * 10;

                for (int y = 0; y < 32; y++)
                {
                    pixel.y = y * 10;

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
