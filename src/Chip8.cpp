#include <fstream>
#include <iostream>
#include <sstream>

#include "Chip8.hpp"

static std::string intToHex(const std::uint16_t &value)
{
    std::stringstream stream;
    stream << std::hex << std::uppercase << "0x" << value;
    return stream.str();
}

namespace chip8
{
    Chip8::Chip8()
        : V{}, I(0), pc(0x200), stack{}, sp(0),
          delay_timer(0), sound_timer(0),
          gfx{}, keypad{}, DrawFlag{false}
    {
    }

    void Chip8::reset()
    {
        memory.fill(0);
        V.fill(0);
        stack.fill(0);
        gfx.fill(0);
        keypad.fill(0);
        I = 0;
        pc = 0x200; // starting from 0x200
        sp = 0;
        delay_timer = 0;
        sound_timer = 0;

        // Loading fontset into memory
        for (size_t i = 0; i < fontset.size(); ++i)
        {
            memory[FONTSET_START_ADDRESS + i] = fontset[i];
        }
    }

    void Chip8::loadROM(const std::string &filename)
    {
        reset();
        // opens the file in binary and sets the position at the end
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            throw std::runtime_error("ROM couldn't be opened: " + filename);
            return;
        }

        // checks the size of the file
        std::streamsize size = file.tellg();

        // returns to the beginning of the file
        file.seekg(0, std::ios::beg);

        // ROM begins at 0x200
        if (size > (4096 - 512))
        {
            throw std::runtime_error("ROM too big! Size: " + std::to_string(size) + " bytes. Max size: " + std::to_string(4096 - 512) + " bytes.");
            return;
        }

        file.read(reinterpret_cast<char *>(&memory[0x200]), size);
        std::cout << "ROM loaded: " << filename << " (" << size << " bytes)" << std::endl;
    }

    std::uint8_t *Chip8::GetKeypad()
    {
        return keypad.data();
    }

    void Chip8::emulateCycle()
    {

        if (pc >= memory.size() - 1) // -1 bo odczytujesz 2 bajty (pc, pc+1)
        {
            throw std::runtime_error("Program counter out of bounds: " + intToHex(pc));
        }

        // Fetch opcode (2 bytes)
        // note: each instruction takies 2 bytes of memory
        std::uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
        std::cout << "PC: " << std::hex << pc << " Opcode: 0x" << opcode << std::dec << '\n';

        // decoding the instruction - checking the first nibble (4 bits)
        switch (opcode & 0xF000)
        {
        case 0x0000:
        {
            switch (opcode)
            {
            case 0x00E0: // CLS – clean the view
                std::cout << "Instruction: CLS (clean the view)\n";
                gfx.fill(0); // clears the screen
                DrawFlag = true;
                pc += 2;
                break;

            case 0x00EE: // RET – returns from a subroutine
                std::cout << "Instruction: RET\n";
                --sp;           // decrement stack pointer
                pc = stack[sp]; // returning to the previously saved address
                pc += 2;        // increment program counter, go to the next instruction
                break;

            case 0x0000: // NOP - does nothing - TEST FEATURE, NORMALLY NOT USED
                std::cout << "Instruction: NOP\n";
                pc += 2;
                break;

            default:
                throw std::runtime_error("Unknown instruction: " + intToHex(opcode));
                break;
            }
            break;
        }

        case 0x1000:
        { // 1NNN: JP addr - JUMP to the address NNN
            std::uint16_t nnn = opcode & 0x0FFF;
            pc = nnn;
            break;
        }

        case 0x2000:
        { // CALL addr - CALL subroutine at NNN
            std::uint16_t nnn = opcode & 0x0FFF;
            stack[sp] = pc; // saves the current address to the stack
            ++sp;           // increament stack pointer
            pc = nnn;       // set the program counter to the address
            std::cout << "Instruction: CALL " << intToHex(nnn) << '\n';
            break;
        }

        case 0x3000: // SE Vx, NN - skip instruction if Vx == NN
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t nn = opcode & 0x00FF;
            std::cout << "Instruction: SE V" << +x << ", " << +nn << "\n";
            if (V[x] == nn)
                pc += 4;
            else
                pc += 2;
            break;
        }

        case 0x4000: // SNE Vx, NN - skip instruction if Vx != NN
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t nn = opcode & 0x00FF;
            std::cout << "Instruction: SNE V" << +x << ", " << +nn << "\n";
            if (V[x] != nn)
                pc += 4;
            else
                pc += 2;
            break;
        }

        case 0x5000: // SE Vx, Vy - skip instruction if Vx == Vy
        {
            if ((opcode & 0x000F) != 0)
            {
                throw std::runtime_error("Invalid instruction: " + intToHex(opcode));
            }

            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t y = (opcode & 0x00F0) >> 4;
            std::cout << "Instruction: SE V" << +x << ", V" << +y << "\n";

            if (V[x] == V[y])
                pc += 4;
            else
                pc += 2;
            break;
        }

        case 0x6000: // 6XNN - LD Vx, NN - Loads NN value to Vx register DOES SET
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8; // takes the no of the register
            std::uint8_t nn = opcode & 0x00FF;       // takes the NN
            V[x] = nn;                               // sets the value to the register
            std::cout << "Instruction: LD V" << +x << ", " << +nn << '\n';
            pc += 2;
            break;
        }

        case 0x7000: // 7XNN - ADD Vx, NN - adds NN value to Vx register DOES NOT SET, increments
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t nn = opcode & 0x00FF;
            V[x] += nn; // CHANGES the value of the register
            std::cout << "Instruction: ADD V" << +x << ", " << +nn << '\n';
            pc += 2; //
            break;
        }

        case 0x8000:
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t y = (opcode & 0x00F0) >> 4;
            std::uint8_t subcode = opcode & 0x000F;

            switch (subcode)
            {
            case 0x0: // 8XY0 - LD Vx, Vy - loads the value of Vy to Vx register
                V[x] = V[y];
                std::cout << "Instruction: LD V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;

            case 0x1: // 8XY1 -  OR Vx, Vy - bitwise OR of Vx and Vy registers
            {
                V[x] |= V[y];
                std::cout << "Instruction: OR V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0x2: // 8XY2 - AND Vx, Vy - bitwise AND of Vx and Vy registers
            {
                V[x] &= V[y];
                std::cout << "Instruction: AND V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0x3: // 8XY3 - XOR Vx, Vy - bitwise XOR of Vx and Vy registers
            {
                V[x] ^= V[y];
                std::cout << "Instruction: XOR V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0x4: // 8XY4 - ADD Vx, Vy - adds the value of Vy to Vx register and sets VF if there is a carry
            {
                std::uint16_t sum = V[x] + V[y];
                V[0xF] = (sum > 0xFF) ? 1 : 0; // sets the carry flag
                V[x] = sum & 0xFF;             // sets the value of the register
                std::cout << "Instruction: ADD V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0x5: // 8XY5 - SUB Vx, Vy - subtracts the value of Vy from Vx register and sets VF if there is no borrow
            {
                V[0xF] = (V[x] > V[y]) ? 1 : 0; // sets the borrow flag
                V[x] -= V[y];                   // sets the value of the register
                std::cout << "Instruction: SUB V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0x6: // 8XY6 - SHR Vx {, Vy} - shifts Vx register right by 1 bit and sets VF to the least significant bit of Vx
            {
                V[0xF] = V[x] & 0x1; // sets the least significant bit of Vx to VF
                V[x] >>= 1;          // shifts the value of the register right by 1 bit
                std::cout << "Instruction: SHR V" << +x << '\n';
                pc += 2;
                break;
            }

            case 0x7: // 8XY7 - SUBN Vx, Vy - subtracts the value of Vx from Vy register and sets VF if there is no borrow
            {
                V[0xF] = (V[y] > V[x]) ? 1 : 0; // sets the borrow flag
                V[y] -= V[x];                   // sets the value of the register
                std::cout << "Instruction: SUBN V" << +x << ", V" << +y << '\n';
                pc += 2;
                break;
            }

            case 0xE: // 8XYE - SHL Vx {, Vy} - shifts Vx register left by 1 bit and sets VF to the most significant bit of Vx
            {
                V[0xF] = (V[x] & 0x80) >> 7; // sets the most significant bit of Vx to VF
                V[x] <<= 1;                  // shifts the value of the register left by 1 bit
                std::cout << "Instruction: SHL V" << +x << '\n';
                pc += 2;
                break;
            }

            default:
            {
                throw std::runtime_error("Unsupported 0x8XY* instruction: " + intToHex(opcode));
                break;
            }
            }
            break;
        }

        case 0x9000: // SNE Vx, Vy - skip instruction if Vx != Vy
        {
            if ((opcode & 0x000F) != 0)
            {
                throw std::runtime_error("Invalid instruction: " + intToHex(opcode));
            }

            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t y = (opcode & 0x00F0) >> 4;
            std::cout << "Instruction: SNE V" << +x << ", V" << +y << "\n";

            if (V[x] != V[y])
                pc += 4;
            else
                pc += 2;
            break;
        }

        case 0xA000: // ANNN - LD I, NNN - setting the NNN (12-bit address) to the I register
        {
            std::uint16_t nnn = opcode & 0x0FFF;
            I = nnn;
            std::cout << "Instruction: LD I, " << intToHex(nnn) << '\n';
            pc += 2;
            break;
        }

        case 0xB000: // BNNN - JP V0, NNN - jumps to the address NNN + V0
        {
            std::uint16_t nnn = opcode & 0x0FFF;
            pc = V[0] + nnn; // jumps to the address NNN + V0
            std::cout << "Instruction: JP " << intToHex(nnn) << '\n';
            break;
        }

        case 0xC000: // CXNN - RND Vx, NN - generates a random number and ANDs it with NN
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8; // takes the no of the register
            std::uint8_t nn = opcode & 0x00FF;       // takes the NN
            std::uint8_t randomByte = rand() % 256;  // generates a random byte from 0 to 255
            V[x] = randomByte & nn;                  // sets the value to the register - allows to control the range of generation
            std::cout << "Instruction: RND V" << +x << ", " << +nn << '\n';
            pc += 2;
            break;
        }

        case 0xD000: // DXYN - Draw sprite at (Vx, Vy), N bytes tall
        {
            std::uint8_t x = V[(opcode & 0x0F00) >> 8];
            std::uint8_t y = V[(opcode & 0x00F0) >> 4];
            std::uint8_t height = opcode & 0x000F;
            std::uint8_t pixel;

            V[0xF] = 0; // VF = 0 (flaga kolizji)

            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        int xPos = (x + xline) % 64;
                        int yPos = (y + yline) % 32;
                        int index = yPos * 64 + xPos;

                        if (gfx[index] == 1)
                            V[0xF] = 1; // kolizja!

                        gfx[index] ^= 1; // rysowanie XOR
                    }
                }
            }

            DrawFlag = true;

            std::cout << "DRW V" << +((opcode & 0x0F00) >> 8)
                      << ", V" << +((opcode & 0x00F0) >> 4)
                      << ", " << +height << " → sprite\n";

            pc += 2;
            break;
        }

        case 0xE000: // Key operations
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t key = V[x];

            switch (opcode & 0x00FF)
            {
            case 0x9E: // EX9E - SKP Vx - skip next instruction if key with value of Vx is pressed
            {
                std::cout << "Instruction: SKP V" << +x << '\n';

                if (keypad[key] != 0)
                {
                    pc += 4;
                }

                else
                {
                    pc += 2;
                }

                break;
            }

            case 0xA1: // EXA1 - SKNP Vx - skip next instruction if key with value of Vx is not pressed
            {
                std::cout << "Instruction: SKNP V" << +x << '\n';

                if (keypad[key] == 0)
                {
                    pc += 4;
                }

                else
                {
                    pc += 2;
                }

                break;
            }

            default:
            {
                throw std::runtime_error("Unsupported EX instruction: " + intToHex(opcode));
                break;
            }
            }
            break;
        }

        case 0xF000:
        {
            std::uint8_t x = (opcode & 0x0F00) >> 8;
            std::uint8_t subcode = opcode & 0x00FF;
            std::cout << "Subcode: " << intToHex(subcode) << '\n';

            switch (subcode)
            {
            case 0x07: // FX07 - LD Vx, DT - loads the value of the delay timer to Vx register
            {
                std::cout << "Instruction: LD V" << +x << ", DT\n";
                V[x] = delay_timer;
                pc += 2;
                break;
            }

            case 0x0A: // FX0A - LD Vx, K - waits for a key press and stores the value in Vx register
            {
                std::cout << "Instruction: LD V" << +x << ", K\n";

                bool keyPressed = false;

                for (std::size_t i = 0; i < 16; i++)
                {
                    if (keypad[i] != 0)
                    {
                        V[x] = i;
                        keyPressed = true;
                        break;
                    }
                }

                if (!keyPressed)
                {
                    return; // wait for a key press
                }

                pc += 2;
                break;
            }

            case 0x15: // FX15 - LD DT, Vx - sets the delay timer to the value of Vx register
            {
                std::cout << "Instruction: LD DT, V" << +x << "\n";
                delay_timer = V[x];
                pc += 2;
                break;
            }

            case 0x18: // FX18 - LD ST, Vx - sets the sound timer to the value of Vx register
            {
                std::cout << "Instruction: LD ST, V" << +x << '\n';
                sound_timer = V[x];
                pc += 2;
                break;
            }

            case 0x1E: // FX1E - ADD I, Vx - adds the value of Vx register to I register
            {
                std::cout << "Instruction: ADD I, V" << +x << '\n';
                I += V[x];
                pc += 2;
                break;
            }

            case 0x29: // FX29 - LD F, Vx - sets I to the location of the sprite for the character in Vx register
            {
                I = FONTSET_START_ADDRESS + (V[x] * 5);
                std::cout << "Instruction: LD F, V" << +x << " → I = " << intToHex(I) << '\n';
                pc += 2;
                break;
            }

            case 0x33: // FX33 - LD B, Vx - stores the binary-coded decimal representation of Vx in memory locations I, I+1, and I+2
            {
                std::cout << "Instruction: LD B, V" << +x << '\n';
                memory[I] = V[x] / 100;           // hundreds place
                memory[I + 1] = (V[x] / 10) % 10; // tens place
                memory[I + 2] = V[x] % 10;        // ones place
                pc += 2;
                break;
            }

            case 0x55: // FX55 - LD [I], Vx — Store V0 to Vx in memory starting at I
            {
                std::cout << "Instruction: LD [I], V" << +x << '\n';

                for (std::size_t i = 0; i <= x; ++i)
                {
                    memory[I + i] = V[i];
                }

                pc += 2;
                break;
            }

            case 0x65: // FX65 - LD Vx, [I]
            {
                std::cout << "Instruction: LD V" << +x << ", [I]\n";
                for (std::size_t i = 0; i <= x; ++i)
                {
                    V[i] = memory[I + i];
                }
                // I unchanged
                pc += 2;
                break;
            }

            default:
            {
                throw std::runtime_error("Unsupported FX instruction: " + intToHex(opcode));
                break;
            }
            }
            break;
        }

        default:
        {
            throw std::runtime_error("Not supported opcode: " + intToHex(opcode));
            break;
        }
        }
    }

    const std::uint8_t *Chip8::GetGfx() const
    {
        return gfx.data();
    }

    bool Chip8::ShouldDraw() const
    {
        return DrawFlag;
    }

    void Chip8::ClearDrawFlag()
    {
        DrawFlag = false;
    }
}
