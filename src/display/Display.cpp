#include <iostream>
#include <unordered_map>
#include <cmath>
#include <vector>

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
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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

        const int frequency = 440;
        const int sampleRate = 44100;
        const int durationMs = 100;

        SDL_AudioSpec desiredSpec{};
        desiredSpec.freq = sampleRate;
        desiredSpec.format = AUDIO_U8;
        desiredSpec.channels = 1;
        desiredSpec.samples = 4096;
        desiredSpec.callback = nullptr;

        audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &audioSpec, 0);

        if (audioDevice == 0)
        {
            throw std::runtime_error(std::string("SDL_OpenAudioDevice failed: ") + SDL_GetError());
        }

        else
        {
            int sampleCount = (sampleRate * durationMs) / 1000;
            audioBuffer.resize(sampleCount);

            for (int i = 0; i < sampleCount; ++i)
            {
                double time = static_cast<double>(i) / sampleRate;
                audioBuffer[i] = static_cast<std::uint8_t>(127 * (std::sin(2.0 * M_PI * frequency * time) + 1));
            }
        }
    }

    Display::~Display()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        if (audioDevice != 0)
        {
            SDL_CloseAudioDevice(audioDevice);
        }
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

    void Display::Beep()
    {
        if (audioDevice != 0)
        {
            SDL_ClearQueuedAudio(audioDevice);

            SDL_QueueAudio(audioDevice, audioBuffer.data(), static_cast<Uint32>(audioBuffer.size()));
            SDL_PauseAudioDevice(audioDevice, 0);
        }
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
