#include "cpu.h"

void CPU::reset()
{
    AF = 0x00;
    BC = 0x00;
    DE = 0x00;
    HL = 0x00;
    SP = 0x00;
    PC = 0x00;
}

void NOP(CPU &cpu)
{
    return;
}

typedef void (*Instruction)(CPU &);

Instruction opcode_table[256] = {NOP /* default all to NOP for now */};

void initialize_opcode_table()
{
    opcode_table[0x00] = NOP; // 0x00 opcode is NOP
    // opcode_table[0x01] = LD_BC_d16;
    // ... fill in the rest
}

void execute_instruction(CPU &cpu, uint8_t opcode)
{
    opcode_table[opcode](cpu);
}

Memory memory;
CPU cpu;