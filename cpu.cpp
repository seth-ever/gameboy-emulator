#include "cpu.h"

void NOP(CPU &cpu)
{
    cpu.PC++; // NOP takes 1 byte
}

typedef void (*Instruction)(CPU &);

Instruction opcode_table[256] = {NOP /* default all to NOP for now */};

void initialize_opcode_table()
{
    opcode_table[0x00] = NOP; // 0x00 opcode is NOP
    // opcode_table[0x01] = LD_BC_d16;
    // ... fill in the rest
}

// Execute an instruction
void execute_instruction(CPU &cpu, uint8_t opcode)
{
    opcode_table[opcode](cpu);
}