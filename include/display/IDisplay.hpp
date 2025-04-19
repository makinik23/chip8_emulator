#pragma once

#include <cstdint>

namespace display
{
    /**
     * @brief Interface for the display system of CHIP-8.
     */
    class IDisplay
    {
    public:
        /**
         * @brief Checks if the emulator should be running.
         * @return true if emulator should be running
         * @return false if emulator should stop
         */
        virtual bool IsRunning() const = 0;

        /**
         * @brief Handles the system events.
         */
        virtual void HandleEvents(std::uint8_t *keypad) = 0;

        /**
         * @brief Clears the display.
         * Sets the whole display to black.
         */
        virtual void Clear() = 0;

        /**
         * @brief Renders the graphic buffor.
         * @param Pointer to the 64 x 32 buffor containing 0 and 1.
         */
        virtual void Render(const std::uint8_t *gfx) = 0;

        /**
         * @brief Calls a sound via SDL2.
         */
        virtual void Beep() = 0;

        /**
         * @brief Destructor.
         */
        virtual ~IDisplay() = default;
    };
}
