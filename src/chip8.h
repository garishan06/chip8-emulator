#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>

class Chip8{
    public:
        Chip8();
        void LoadROM(char const* filename);
        void Cycle(); // emulation cycle

        //CHIP8 Emulator Components 
        uint8_t registers [16]{}; //the curly braces at the end initializes all the values in the array to zero
        uint8_t memory [4096]{}; //uint8_t means "unsigned int of 8 bits; the _t is just convention to clarify we're referring to a type name, not a variable name."
        uint16_t ir{}; //index register
        uint16_t pc{}; // program counter
        uint16_t stack [16]{};
        uint8_t sp {}; //stack pointer
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t display [64*32]{};
        uint16_t opcode;




};



























#endif