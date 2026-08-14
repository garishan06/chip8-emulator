#include "chip8.h"
#include <fstream>
#include <cstring>
#include <cstdint>


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
//Open file in binary mode
    std::ifstream file(filename, std::ios::binary);

// get the file size
    file.seekg(0, std::ios::end); // this line takes us to the end of the file
    int size = file.tellg();
    file.seekg(0,std::ios::beg);
//create the buffer
    char* buff = new char[size];


//read the file to the buffer
    file.read(buff, size);
//close file
    file.close();
//delete the buffer
    delete buff;
}

void Chip8::Cycle(){
//FETCH OPCODE

//DECODE OPCODE

//EXECUTE OPCODE


}




//INSTRUCTION IMPLEMENTATIONS

void Chip8::Opcode_00E0(){ // Clear Display
    memset(display, 0, sizeof(display)); // this function is used to fill the entire array with any value you want. In our case, we fill the display with zeroes to clear the display
}

void Chip8::Opcode_00EE(){ //Return from subroutine
    pc = stack[--sp]; // since the stack pointer always points at a value of 0x000, the top of the stack is actually stored 1 idx before the stack pointer. We need to decrement the stack and then return the new value to pc, which is why I did --sp instead of sp--. sp-- returns the old value and then decrements sp.
}

void Chip8::Opcode_1nnn(){ // JUMP to the address at 0x0nnn
    pc = opcode & 0x0FFF; // we use bitmasking herre to remove the top 4 bits and keep the bottom 12 which contain the info we want on the address.
}

void Chip8::Opcode_2nnn(){ // CALL a subroutined at 0x0nnn
    stack[sp] = pc; //the pc should be incremented before calling this function so that we're pointing to the correct location on the stack. Otherwise, we'd create an infinite loop.
    sp++;
    pc = opcode & 0x0FFF;
}

void Chip8::Opcode_3xkk(){ //SKIP next instruction if Vx == kk
    uint8_t Vx = (opcode & 0x0F00) >> 8; // here, we're keeping bits 8-11 and shifting them all the way to the right so we can use them as an index position to access the correct register.
    uint8_t byte = opcode & 0x00FF;

    if (registers[Vx] == byte){
        pc+= 2; // since instructions are 2 bytes long, we have to increment by 2
    }
}

void Chip8::Opcode_4xkk(){ // SKIP next instruction if Vx != kk
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    if (registers[Vx] != byte){
        pc+=2;
    }
}

void Chip8::Opcode_5xy0(){ // SKIP next instruction if Vx == Vy
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if (registers[Vx] != registers[Vy]){
        pc+= 2;
    }
}

void Chip8::Opcode_6xkk(){ // LOAD byte kk into Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    
    registers[Vx] = byte;
}

void Chip8::Opcode_7xkk(){ // INCREMENT Vx by byte kk.
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;
    
    registers[Vx] += byte;
    
}

void Chip8::Opcode_8xy0(){ //SET Vx = Vy
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    
    registers[Vx] = registers[Vy];
}

void Chip8::Opcode_8xy1(){ //SET Vx = Vx (bitwise OR) Vy
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::Opcode_8xy2(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] & registers[Vy];
}