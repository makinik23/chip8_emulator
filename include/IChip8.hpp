#pragma once

#include <string>

namespace chip8
{
    /**
     * @class IChip
     * @brief Interface for the Chip8 emulator.
     */
    class IChip
    {
    public:
        /**
         * @brief resets the Chip8 emulator.
         */
        virtual void reset() = 0;

        /**
         * @brief Loads a ROM file into the emulator.
         * @param filename The name of the ROM file to load.
         */
        virtual void loadROM(const std::string &filename) = 0;

        /**
         * @brief Emulates one cycle of the Chip8 CPU.
         */
        virtual void emulateCycle() = 0;

        /**
         * @brief Returns pointer to the graphics buffer.
         * @return Pointer to the graphics buffer (64 x 32).
         */
        virtual const std::uint8_t *GetGfx() const = 0;

        /**
         * @brief Checks if the screen should be refreshed.Chip8
         * @return true if the screen should be refreshed.
         */
        virtual bool ShouldDraw() const = 0;

        /**
         * @brief Clears the draw flag.
         */
        virtual void ClearDrawFlag() = 0;

        /**
         * @brief Destructor.
         */
        virtual ~IChip() = default;
    };
}
