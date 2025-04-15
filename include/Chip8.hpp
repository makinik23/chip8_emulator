#pragma once

#include <array>
#include <cstdint>
#include <string>

#include "IChip8.hpp"

namespace chip8
{
    /**
     * @class Chip8
     * @brief Main class implementing the Chip8 emulator.
     */
    class Chip8 final : public IChip
    {
    public:
        /**
         * @brief Constructor for the Chip8 class.
         */
        explicit Chip8();

        void reset() override;
        void loadROM(const std::string &filename) override;
        void emulateCycle() override;
        bool ShouldDraw() const override;
        void ClearDrawFlag() override;
        const std::uint8_t *GetGfx() const override;

    private:
        /**
         * @brief Main RAM (4 kB).
         */
        std::array<uint8_t, 4096> memory{};

        /**
         * @brief Registers (16 registers of 8 bits each).
         */
        std::array<uint8_t, 16> V{};

        /**
         * @brief Adress register (16 bits).
         */
        std::uint16_t I = 0;

        /**
         * @brief Program counter (16 bits).
         */
        std::uint16_t pc = 0x200;

        /**
         * @brief Stack (16 levels).
         */
        std::array<std::uint16_t, 16> stack{};

        /**
         * @brief Stack pointer (8 bits).
         */
        std::uint8_t sp = 0;

        /**
         * @brief Delay timer (8 bits).
         */
        std::uint8_t delay_timer = 0;

        /**
         * @brief Sound timer (8 bits).
         */
        std::uint8_t sound_timer = 0;

        /**
         * @brief Screen (64x32 pixels).
         * Each pixel is represented by a single byte (0 or 1).
         */
        std::array<uint8_t, 64 * 32> gfx{};

        /**
         * @brief Keypad state (16 keys).
         */
        std::array<uint8_t, 16> keypad{};

        /**
         * @brief Flag indicating if the screen should be redrawn.
         */
        bool DrawFlag = true;

        /**
         * @brief Fontset (5x8 pixels for each character).
         * The fontset is stored in the memory starting from address 0x50.
         * Used to draw hexadecimal digits (0-F).
         */
        static constexpr std::array<std::uint8_t, 80> fontset = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

        /**
         * @brief Address where fontset is stored in memory.
         */
        static constexpr std::uint16_t FONTSET_START_ADDRESS = 0x050;
    };
}
