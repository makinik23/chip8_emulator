#include <iostream>
#include <unordered_map>

#include "display/Display.hpp"

namespace
{
    const std::unordered_map<SDL_Keycode, std::uint8_t> keyMap = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC}, {SDLK_q, 0x4}, {SDLK_w, 0x5}, {SDLK_e, 0x6}, {SDLK_r, 0xD}, {SDLK_a, 0x7}, {SDLK_s, 0x8}, {SDLK_d, 0x9}, {SDLK_f, 0xE}, {SDLK_z, 0xA}, {SDLK_x, 0x0}, {SDLK_c, 0xB}, {SDLK_v, 0xF}};
}

namespace display
{
    Display::Display()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }

        window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WIDTH * SCALE,
            HEIGHT * SCALE,
            SDL_WINDOW_SHOWN);

        if (!window)
        {
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
        }
    }

    Display::~Display()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Display::Clear()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    void Display::Render(const uint8_t *gfx)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                if (gfx[y * WIDTH + x])
                {
                    SDL_Rect pixel = {x * SCALE, y * SCALE, SCALE, SCALE};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    void Display::HandleEvents(std::uint8_t *keypad)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }

            else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                bool isPressed = (event.type == SDL_KEYDOWN);
                auto it = keyMap.find(event.key.keysym.sym);

                if (it != keyMap.end())
                {
                    std::uint8_t chip8Key = it->second;
                    keypad[chip8Key] = isPressed ? 1 : 0;
                }
            }
        }
    }

    bool Display::IsRunning() const
    {
        return running;
    }
}
