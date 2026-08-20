#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <chrono>
#include <random> // for RNG stuff

#define DISPLAY_WIDTH 64u
#define DISPLAY_HEIGHT 32u
#define FONTSET_START_ADDRESS 0x50
#define ROM_START_ADDRESS 0x200
class Chip8{
    public:
        Chip8();
        void LoadROM(char const* filename);
        void Cycle(); // emulation cycle


        // Opcode Functions
        void Opcode_00E0();
        void Opcode_00EE();
        void Opcode_1nnn();
        void Opcode_2nnn();
        void Opcode_3xkk();
        void Opcode_4xkk();
        void Opcode_5xy0();
        void Opcode_6xkk();
        void Opcode_7xkk();
        void Opcode_8xy0();
        void Opcode_8xy1();
        void Opcode_8xy2();
        void Opcode_8xy3();
        void Opcode_8xy4();
        void Opcode_8xy5();
        void Opcode_8xy6();
        void Opcode_8xy7();
        void Opcode_8xyE();
        void Opcode_9xy0();
        void Opcode_Annn();
        void Opcode_Bnnn();
        void Opcode_Cxkk();
        void Opcode_Dxyn();
        void Opcode_Ex9E();
        void Opcode_EXA1();
        void Opcode_Fx07();
        void Opcode_Fx0A();
        void Opcode_Fx15();
        void Opcode_Fx18();
        void Opcode_FX1E();
        void Opcode_Fx29();
        void Opcode_Fx33();
        void Opcode_Fx55();
        void Opcode_Fx65();
        void Opcode_NULL();

       
        

        //CHIP8 Emulator Components 
        uint8_t registers [16]{}; //the curly braces at the end initializes all the values in the array to zero
        uint8_t memory [4096]{}; //uint8_t means "unsigned int of 8 bits; the _t is just convention to clarify we're referring to a type name, not a variable name."
        uint16_t ir{}; //index register
        uint16_t pc{}; // program counter
        uint16_t stack [16]{};
        uint8_t sp {}; //stack pointer; it should start at idx 0 since the stack should initially be empty.

        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t display [64*32]{};
        uint16_t opcode;

        //RNG Variables
        std::default_random_engine randGen;
	    std::uniform_int_distribution<uint8_t> randByte;


        //Function Pointer Table Variables
        typedef void (Chip8::* Chip8FuncPtr)();

         //These are methods for the Function pointer Table, since the main table contains sub tables. Therefore the main table has to point to functions responsible for the subtables
        void Table0();
        void Table8();
        void TableE();
        void TableF();
        
        Chip8FuncPtr table[16]; // the main table indexed by the first nibble of the opcode
        Chip8FuncPtr table0[0xE + 1]; //subtable for opcodes starting in 0x0
        Chip8FuncPtr table8[0xE + 1]; //subtable for opcodes starting in 0x8
        Chip8FuncPtr tableE[0xE + 1]; //sub table for opcodes starting in 0xE
        Chip8FuncPtr tableF[0x65 + 1]; // substalbe for opcodes starting in 0xF




};



























#endif