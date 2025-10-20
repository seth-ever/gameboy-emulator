#include "cpu.h"

Memory memory;
CPU cpu;

void NOP(CPU &cpu, Memory &mem, uint8_t opcode)
{
    cpu.PC += 1;
    cpu.cycles += 4;
}

// ld_DESTINATION_SOURCE

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

    cpu.PC += 3;
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

    cpu.PC += 1;
    cpu.cycles += 8;
}

void ld_a_r16mem(CPU &cpu, Memory &mem, uint8_t opcode)
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

    cpu.A = mem.read(address);

    cpu.PC += 1;
    cpu.cycles += 8;
}

void ld_imm16_sp(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint16_t adress = mem.read(cpu.PC + 1) | (mem.read(cpu.PC + 2) << 8);
    mem.writeWord(adress, cpu.SP);

    cpu.PC += 3;
    cpu.cycles += 20;
}

void inc_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode & 0x30) >> 4;

    switch (regVal)
    {
    case 0:
        cpu.BC++;
        break;
    case 1:
        cpu.DE++;
        break;
    case 2:
        cpu.HL++;
        break;
    case 3:
        cpu.SP++;
        break;
    }

    cpu.PC += 1;
    cpu.cycles += 8;
}

void dec_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode & 0x30) >> 4;

    switch (regVal)
    {
    case 0:
        cpu.BC--;
        break;
    case 1:
        cpu.DE--;
        break;
    case 2:
        cpu.HL--;
        break;
    case 3:
        cpu.SP--;
        break;
    }

    cpu.PC += 1;
    cpu.cycles += 8;
}

void add_hl_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode & 0x30) >> 4;
    uint16_t value;

    switch (regVal)
    {
    case 0:
        value = cpu.BC;
        break;
    case 1:
        value = cpu.DE;
        break;
    case 2:
        value = cpu.HL;
        break;
    case 3:
        value = cpu.SP;
        break;
    }

    uint32_t result = cpu.HL + value;

    // Flags
    cpu.setSubtract(false);                                            // Always clear
    cpu.setHalfCarry(((cpu.HL & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF); // if carry from 11 -> 12
    cpu.setCarry(result > 0xFFFF);                                     // if carry from bit 15

    cpu.HL = static_cast<uint16_t>(result);

    cpu.PC += 1;
    cpu.cycles += 8;
}

typedef void (*Instruction)(CPU &, Memory &, uint8_t opcode);

Instruction opcode_table[256] = {NOP /* default all to NOP for now */};

void initialize_opcode_table()
{
    opcode_table[0x00] = NOP;

    for (uint8_t i = 0x01; i <= 0x31; i += 0x10) // ld_r16_imm16
    {
        opcode_table[i] = ld_r16_imm16;
    }

    for (uint8_t i = 0x02; i <= 0x32; i += 0x10) // ld_r16mem_a
    {
        opcode_table[i] = ld_r16mem_a;
    }

    for (uint8_t i = 0x0A; i <= 0x3A; i += 0x10) // ld_a_r16mem
    {
        opcode_table[i] = ld_a_r16mem;
    }

    opcode_table[0x08] = ld_imm16_sp;

    for (uint8_t i = 0x03; i <= 0x33; i += 0x10) // inc_r16
    {
        opcode_table[i] = inc_r16;
    }

    for (uint8_t i = 0x0B; i <= 0x3B; i += 0x10) // dec_r16
    {
        opcode_table[i] = dec_r16;
    }

    for (uint8_t i = 0x09; i <= 0x39; i += 0x10) // add_hl_r16
    {
        opcode_table[i] = add_hl_r16;
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
