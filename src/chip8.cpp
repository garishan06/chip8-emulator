#include "chip8.h"
#include <fstream>



Chip8::Chip8()
    // this is an initializer list. Its a special C++ syntax that allows u to initialize variables before the constructor runs. Instead of filling variables with garbage values and then assigning them values, this allows you to set their values immediately
     : randGen(std::chrono::system_clock::now().time_since_epoch().count()), // this is for seeding; we seed the generator with the computer's time so we dont have the same seed for every run.
        pc(0x200) // the program counter should start at memory address 0x200 since this is where the start of the ROM instructions are   
{
    // INITIALIZING VARIABLES 
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // LOADING THE FONT
    const unsigned int FONTSET_SIZE = 80; // since each character is created on a 5x5 grid, we need 5 bytes for each character. Since there are 16 total characters that must be stored in memory, this requires the fontset to contain 16*5 = 80 bytes of space
    const unsigned int FONTSET_START_ADDRESS = 0x50; // the fonts are stored in memory from 0x050 - 0x0A0.

    const uint8_t FONTSET [FONTSET_SIZE] = { // each character takes up a 5x5 bitsize grid. The rightmost column on this grid is left empty as a seperator between adjacent characters. Since each row only uses 5 out of the 8 bits in a byte, the 3 rightmost bits are left as zeroes. 
        0xF0, 0x90, 0x90, 0x90, 0xF0, //character 0
        0x20,0x60, 0x20, 0x20, 0x70,  //character 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //character 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //character 3
        0x90, 0x90, 0xF0, 0x10, 0x10, //character 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //character 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //character 6
        0xF0, 0x10, 0x20, 0x40, 0x40, //character 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //character 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //character 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //character A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //character B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //character C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //character D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //character E
        0xF0, 0x80, 0xF0, 0x80, 0x80, //character F
    }; 


    for(int i = 0; i < FONTSET_SIZE; i++){ //stores the fontset in the correct memory location
        memory[FONTSET_START_ADDRESS + i] = FONTSET[i];
    }
}



void Chip8::LoadROM(const char* filename){
//OPen file in binary mode

// get the file size

//create the buffer
//read the file to the buffer

//save the buffer into memory

//close file

//delete the buffer
}

void Chip8::Cycle(){
//FETCH OPCODE

//DECODE OPCODE

//EXECUTE OPCODE


}