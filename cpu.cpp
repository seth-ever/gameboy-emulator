#include "cpu.h"

Memory memory;
CPU cpu;

void NOP(CPU &cpu, Memory &mem)
{
}

typedef void (*Instruction)(CPU &, Memory &);

Instruction opcode_table[256] = {NOP /* default all to NOP for now */};

void initialize_opcode_table()
{
    opcode_table[0x00] = NOP; // 0x00 opcode is NOP
    // opcode_table[0x01] = LD_BC_d16;
    // ... fill in the rest
}

void CPU::reset()
{
    AF = 0x00;
    BC = 0x00;
    DE = 0x00;
    HL = 0x00;
    SP = 0x00;
    PC = 0x00;
}

void CPU::step(Memory &memory)
{
    uint8_t opcode = memory.read(cpu.PC);
    opcode_table[opcode](*this, memory);
}
