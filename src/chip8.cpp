#include "chip8.h"
#include <fstream>
#include <cstring>
#include <cstdint>

#define DISPLAY_WIDTH 64u
#define DISPLAY_HEIGHT 32u
#define FONTSET_START_ADDRESS 0x50
#define ROM_START_ADDRESS 0x200
Chip8::Chip8()
    // this is an initializer list. Its a special C++ syntax that allows u to initialize variables before the constructor runs. Instead of filling variables with garbage values and then assigning them values, this allows you to set their values immediately
     : randGen(std::chrono::system_clock::now().time_since_epoch().count()), // this is for seeding; we seed the generator with the computer's time so we dont have the same seed for every run.
        pc(0x200) // the program counter should start at memory address 0x200 since this is where the start of the ROM instructions are   
{
    // INITIALIZING VARIABLES 
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    // LOADING THE FONT
    const unsigned int FONTSET_SIZE = 80; // since each character is created on a 5x5 grid, we need 5 bytes for each character. Since there are 16 total characters that must be stored in memory, this requires the fontset to contain 16*5 = 80 bytes of space
    

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



    //FUNCTION POINTER TABLE STUFF

    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::Opcode_1nnn;
    table[0x2] = &Chip8::Opcode_2nnn;
    table[0x3] = &Chip8::Opcode_3xkk;
    table[0x4] = &Chip8::Opcode_4xkk;
    table[0x5] = &Chip8::Opcode_5xy0;
    table[0x6] = &Chip8::Opcode_6xkk;
    table[0x7] = &Chip8::Opcode_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::Opcode_9xy0;
    table[0xA] = &Chip8::Opcode_Annn;
    table[0xB] = &Chip8::Opcode_Bnnn;
    table[0xC] = &Chip8::Opcode_Cxkk;
    table[0xD] = &Chip8::Opcode_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    for(int i = 0; i <= 0xE; i++){  //we can use these for loops to initialize all other potential opcodes that we don't need to consider, to point to the null function
        table0[i] = &Chip8::Opcode_NULL;
        table8[i] = &Chip8::Opcode_NULL;
        tableE[i] = &Chip8::Opcode_NULL;
    }

    for(int i = 0; i <= 0x65; i++){ //Since the opcodes that start with F go all the way to Fx65, we're using a seperate for loop to iterate through the F opcodes.
        tableF[i] = &Chip8::Opcode_NULL;
    }

    table0[0x0] = &Chip8::Opcode_00E0;
    table0[0xE] = &Chip8::Opcode_00EE;

    table8[0x0] = &Chip8::Opcode_8xy0;
    table8[0x1] = &Chip8::Opcode_8xy1;
    table8[0x2] = &Chip8::Opcode_8xy2;
    table8[0x3] = &Chip8::Opcode_8xy3;
    table8[0x4] = &Chip8::Opcode_8xy4;
    table8[0x5] = &Chip8::Opcode_8xy5;
    table8[0x6] = &Chip8::Opcode_8xy6;
    table8[0x7] = &Chip8::Opcode_8xy7;
    table8[0xE] = &Chip8::Opcode_8xyE;

    tableE[0x1] = &Chip8::Opcode_EXA1;
    tableE[0xE] = &Chip8::Opcode_Ex9E;

    tableF[0x07] = &Chip8::Opcode_Fx07;
    tableF[0x0A] = &Chip8::Opcode_Fx0A;
    tableF[0x15] = &Chip8::Opcode_Fx15;
    tableF[0x18] = &Chip8::Opcode_Fx18;
    tableF[0x1E] = &Chip8::Opcode_FX1E;
    tableF[0x29] = &Chip8::Opcode_Fx29;
    tableF[0x33] = &Chip8::Opcode_Fx33;
    tableF[0x55] = &Chip8::Opcode_Fx55;
    tableF[0x65] = &Chip8::Opcode_Fx65;
}


void Chip8::Table0(){ 
	(this ->*(table0[opcode & 0x000Fu]))(); // for this syntax, we have to write 'this->' since the function pointer table doesn't automatically know which object the addresses are linked to. The second part after the second asterisk is responsible for calling the correct function in the function pointer table.
}
void Chip8::Table8(){
    (this ->*(table8[opcode &0x000Fu]))();
}
void Chip8::TableE(){
    (this ->*(tableE[opcode &0x000Fu]))();
}
void Chip8::TableF(){
    (this ->*(tableF[opcode &0x00FFu]))();
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

// store contents in memory
    for (int i = 0; i < size; i++){
        memory[ROM_START_ADDRESS + i] = buff[i];
    }

//delete the buffer
    delete [] buff;
}

void Chip8::Cycle(){

    opcode = memory[pc] << 8 | memory[pc+1]; // this combines the 2 halves of the instructions together.

    //we have to increment the pc before we move on bcs some opcodes adjust the pc value accordingly
    pc += 2;

    //run the opcode
    (this ->*table[(opcode & 0xF000u) >> 12])(); //the main table uses the most significant nibble to navigate to the corresponding opcode.
    //adjust timers if necessary
    if(delayTimer > 0){
        --delayTimer;
    }
    if (soundTimer > 0){
        --soundTimer;
    }
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

    if (registers[Vx] == registers[Vy]){
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

void Chip8::Opcode_8xy3(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] ^ registers[Vy];

}

void Chip8::Opcode_8xy4(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    uint16_t sum = registers[Vx] + registers[Vy];

    registers[0xF] = (sum > 0xFF); // we don't need the ternary statement like we use below because in C++, true converts to 1 and false evaluates as a 0.
    registers[Vx] = (sum > 0xFF) ? (sum & 0x00FF) : sum; // if its bigger than 0xFF, that means the sum would be longer than 8 bits. We only want to store the bottom 8 bits in Vx.

}

void Chip8::Opcode_8xy5(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[0xF] = (registers[Vx] > registers[Vy]);
    registers[Vx] -= registers[Vy];
}

void Chip8::Opcode_8xy6(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = registers[Vx] & 0x01; // we must save the old LSB before it falls off when we shift to the right.

    registers[Vx] /= 2; // a bit shift in the right direction meanns we have to divide by 2, which makes sense because a bit shifting to the right should make the resulting number smaller.
}

void Chip8::Opcode_8xy7(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[0xF] = (registers[Vy] > registers[Vx]);
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::Opcode_8xyE(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    registers[0xF] = (registers[Vx] & 0x80) >> 7;

    registers[Vx] = registers[Vx]  << 1; 
}

void Chip8::Opcode_9xy0(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if (registers[Vx] != registers[Vy]){
        pc+=2;
    }
}

void Chip8::Opcode_Annn(){
    ir = opcode & 0x0FFF;
}

void Chip8::Opcode_Bnnn(){
    pc = registers[0] + (opcode & 0x0FFF);
}

void Chip8::Opcode_Cxkk(){
    uint8_t byte = opcode & 0x00FF;
    uint8_t random = randByte(randGen);
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    registers[Vx] = random & byte;

}

void Chip8::Opcode_Dxyn(){

    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    uint8_t n = (opcode & 0x000F); 

    registers[0xF] = 0;
    
    for (int row = 0; row < n; row++){
        uint8_t spriteRow =  memory[ir + row];

        for (int col = 0; col < 8; col++){ // for iterating through each pixel
            uint8_t spritePixel = spriteRow & (0x80 >> col); // we do this to look at one pixel at a time, starting from the MSB in the row going to the LSB
            
             // getting the coordinates of the current pixel, considering wraparound as well. 
            int x = (Vx + col) % DISPLAY_WIDTH;
            int y = (Vy + row) % DISPLAY_HEIGHT;
            
            uint32_t* displayPixel = &display[y* DISPLAY_WIDTH + x];
            
            
            if (spritePixel){ //drawing logic
                if (*displayPixel == 0xFFFFFFFF){ // if the display pixel is already on, then we need to set the Flag Register on
                    registers[0xF] = 1;

                } 

                *displayPixel ^=0xFFFFFFFF;
            }
        
        
        
        }
    }
    
}

void Chip8::Opcode_Ex9E(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];

    if (keypad[key]){
        pc+=2;
    }
}

void Chip8::Opcode_EXA1(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t key = registers[Vx];

    if (!keypad[key]){
        pc+=2;
    }
}

void Chip8::Opcode_Fx07(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    registers[Vx] = delayTimer;
}

void Chip8::Opcode_Fx0A(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    int pressFlag = 0;
    for (int i = 0; i < 16; i++){
        if(keypad[i]){
            pressFlag = 1;
            registers[Vx] = i;
            break;
        }
    }
    if(!pressFlag){ // if none of the keys were pressed, then we need to run the instruction again, which is why we decrement the program counter by 2. 
        pc -=2;
    }

}

void Chip8::Opcode_Fx15(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    
    delayTimer = registers[Vx];

}

void Chip8::Opcode_Fx18(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    soundTimer = registers[Vx];

}

void Chip8::Opcode_FX1E(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    ir += registers[Vx];
}

void Chip8::Opcode_Fx29(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    ir = FONTSET_START_ADDRESS + (registers[Vx] *5);
}

void Chip8::Opcode_Fx33(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    
    uint8_t num = registers[Vx]; // here, I chose to use a seperate variable instead of directly calling registers[Vx] to prevent changing the value of whats stored in the register.

    memory[ir + 2] = num % 10; // saving the 1s digit
    num /=10;
    memory[ir + 1] = num % 10; //saving the 10s digit
    num /= 10;
    memory [ir] = num % 10;    // saving the 100s digit
}

void Chip8::Opcode_Fx55(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    
    for (int y = 0; y <= Vx; y++){
        memory[ir + y] = registers[y];
    }
}

void Chip8::Opcode_Fx65(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    
    for (int y = 0; y <= Vx; y++){
        registers[y] = memory [ir + y];
    }
}

void Chip8::Opcode_NULL(){
    //Nothing here. This additional opcode is used for handling errors and invalid opcodes
}


