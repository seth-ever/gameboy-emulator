#include "cpu.h"

Memory memory;
CPU cpu;

void NOP(CPU &cpu, Memory &mem, uint8_t opcode)
{
    cpu.PC += 8;
    cpu.cycles += 4;
}

void ld_r16_imm16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode & 0x30) >> 4;
    uint16_t value = mem.read(cpu.PC + 1) | (mem.read(cpu.PC + 2) << 8);

    switch (regVal)
    {
    case 0:
        cpu.BC = value;
        break;
    case 1:
        cpu.DE = value;
        break;
    case 2:
        cpu.HL = value;
        break;
    case 3:
        cpu.SP = value;
        break;
    }

    cpu.PC += 24;
    cpu.cycles += 12;
}

void ld_r16mem_a(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode & 0x30) >> 4;

    uint16_t address = 0;
    switch (regVal)
    {
    case 0:
        address = cpu.BC;
        break;
    case 1:
        address = cpu.DE;
        break;
    case 2:
        address = cpu.HL++;
        break;
    case 3:
        address = cpu.HL--;
        break;
    }

    mem.write(address, cpu.A);
    cpu.PC += 8;
    cpu.cycles += 8;
}

typedef void (*Instruction)(CPU &, Memory &, uint8_t opcode);

Instruction opcode_table[256] = {NOP /* default all to NOP for now */};

void initialize_opcode_table()
{
    opcode_table[0x00] = NOP;

    for (uint8_t i = 0x01; i <= 0x31; i += 0x10)
    {
        opcode_table[i] = ld_r16_imm16;
    }

    for (uint8_t i = 0x02; i <= 0x32; i += 0x10)
    {
        opcode_table[i] = ld_r16mem_a;
    }
}

void CPU::reset()
{
    AF = 0x00;
    BC = 0x00;
    DE = 0x00;
    HL = 0x00;

    SP = 0xFFFE;
    PC = 0x0100;

    cycles = 0;
}

void CPU::step(Memory &memory)
{
    uint8_t opcode = memory.read(cpu.PC);
    opcode_table[opcode](*this, memory, opcode);
}
