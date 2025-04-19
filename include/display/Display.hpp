#pragma once

#include <SDL.h>
#include <vector>

#include "IDisplay.hpp"

namespace display
{
    /**
     * @class Display
     * @brief Class implementing the display system of CHIP-8.
     */
    class Display final : public IDisplay
    {
    public:
        static constexpr int WIDTH = 64;
        static constexpr int HEIGHT = 32;
        static constexpr int SCALE = 10; // Scale factor for the display

        /**
         * @brief Constructor for the Display class.
         */
        explicit Display();
        ~Display() override;

        void Clear() override;
        void Render(const std::uint8_t *gfx) override;
        bool IsRunning() const override;
        void HandleEvents(std::uint8_t *keypad) override;
        void Beep() override;

    private:
        SDL_Window *window = nullptr;
        SDL_Renderer *renderer = nullptr;
        bool running = true;
        SDL_AudioDeviceID audioDevice = 0;
        SDL_AudioSpec audioSpec{};
        std::vector<std::uint8_t> audioBuffer;
    };
}
