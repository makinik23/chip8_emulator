# CHIP-8 Emulator

A simple yet fully functional emulator for the classic CHIP-8, written in C++ with SDL2 support.

## 🧠 About

CHIP-8 is a virtual machine developed in the 1970s for learning and writing simple video games. This emulator includes:

- Full support for all 35 CHIP-8 opcodes
- 64x32 monochrome pixel display using SDL2
- 16-key hexadecimal keypad
- Sound support via SDL2
- Timer synchronization at ~60Hz

## ⚙️ Requirements

- C++17 or later
- SDL2 library (headers + binaries)
- CMake 3.10+

## 📆 Config

- Configure ```CMakePresets.json``` to Your Liking
- Feel free to change ```CMakeLists``` files if needed
- Build the project

## ▶️ Running

- Create ```roms``` folder
- Download sample ```.ch8``` files

```bash
./build/chip8_emulator.exe ./roms/<ROM_file_to_be_loaded>.ch8
```

## ⌨️ Key Mapping

CHIP-8       | Keyboard
------------ | ---------
1 2 3 C       | 1 2 3 4
4 5 6 D       | Q W E R
7 8 9 E       | A S D F
A 0 B F       | Z X C V
