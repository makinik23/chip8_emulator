#include <iostream>
#include <memory>
#include <filesystem>

#include "Chip8.hpp"
#include "display/Display.hpp"

inline static int Run(std::unique_ptr<display::IDisplay> display, std::unique_ptr<chip8::IChip> chip) // Change if needed
{
    const int cycleDelayMs = 4;

    while (display->IsRunning())
    {
        Uint32 frameStart = SDL_GetTicks();

        chip->emulateCycle();

        if (chip->ShouldDraw())
        {
            display->Render(chip->GetGfx());
            chip->ClearDrawFlag();
        }

        display->HandleEvents(chip->GetKeypad());
        chip->UpdateTimers();

        if (chip->GetSoundTimer() > 0)
        {
            display->Beep();
        }

        Uint32 frameTime = SDL_GetTicks() - frameStart;

        if (frameTime < cycleDelayMs)
        {
            SDL_Delay(cycleDelayMs - frameTime);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Usage: " << argv[0] << " <ROM_file>" << std::endl;
            return 1;
        }

        std::string romPath = argv[1];
        if (!std::filesystem::exists(romPath))
        {
            std::cerr << "Error: File does not exist: " << romPath << std::endl;
            return 1;
        }

        // Creating instances via interfaces to avoid code duplication
        // and to allow for easy swapping of display implementations
        auto display = std::make_unique<display::Display>();
        auto chip = std::make_unique<chip8::Chip8>();

        chip->loadROM(romPath);

        // Does not duplicate the object
        // A smart pointer can not be copied
        return Run(std::move(display), std::move(chip));

        // After move display and chip are nullptr
    }
    catch (const std::exception &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
