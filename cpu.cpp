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
    uint8_t regVal = (opcode >> 4) & 0x03;
    uint16_t value = mem.read(cpu.PC + 1) | (mem.read(cpu.PC + 2) << 8);

    cpu.setRegister16(regVal, value);

    cpu.PC += 3;
    cpu.cycles += 12;
}

void ld_r16mem_a(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 4) & 0x03;

    uint16_t address = cpu.getRegister16mem(regVal);
    if (regVal == 2)
        cpu.HL++;
    else if (regVal == 3)
        cpu.HL--;

    mem.write(address, cpu.A);

    cpu.PC += 1;
    cpu.cycles += 8;
}

void ld_a_r16mem(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 4) & 0x03;

    uint16_t address = cpu.getRegister16mem(regVal);
    if (regVal == 2)
        cpu.HL++;
    else if (regVal == 3)
        cpu.HL--;

    cpu.A = mem.read(address);

    cpu.PC += 1;
    cpu.cycles += 8;
}

void ld_imm16_sp(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint16_t address = mem.read(cpu.PC + 1) | (mem.read(cpu.PC + 2) << 8);
    mem.writeWord(address, cpu.SP);

    cpu.PC += 3;
    cpu.cycles += 20;
}

void inc_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 4) & 0x03;

    uint16_t value = cpu.getRegister16(regVal);
    cpu.setRegister16(regVal, (value + 1));

    cpu.PC += 1;
    cpu.cycles += 8;
}

void dec_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 4) & 0x03;

    uint16_t value = cpu.getRegister16(regVal);
    cpu.setRegister16(regVal, (value - 1));

    cpu.PC += 1;
    cpu.cycles += 8;
}

void add_hl_r16(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 4) & 0x03;
    uint16_t value = cpu.getRegister16(regVal);

    uint32_t result = cpu.HL + value;

    // Flags
    cpu.setSubtract(false);
    cpu.setHalfCarry(((cpu.HL & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF);
    cpu.setCarry(result > 0xFFFF);

    cpu.HL = static_cast<uint16_t>(result);

    cpu.PC += 1;
    cpu.cycles += 8;
}

void inc_r8(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 3) & 7;
    uint8_t value = cpu.getRegister8(regVal, mem);

    uint16_t result = value + 1;

    cpu.setZero(result == 0);
    cpu.setSubtract(false);
    cpu.setHalfCarry((value & 0x0F) + 1 > 0x0F);

    cpu.setRegister8(regVal, static_cast<uint8_t>(result), mem);

    cpu.PC += 1;
    if (regVal == 6)
        cpu.cycles += 12;
    else
        cpu.cycles += 4;
}

void dec_r8(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 3) & 7;
    uint8_t value = cpu.getRegister8(regVal, mem);

    uint16_t result = value - 1;

    cpu.setZero(result == 0);
    cpu.setSubtract(true);
    cpu.setHalfCarry((value & 0x0F) == 0x00);

    cpu.setRegister8(regVal, static_cast<uint8_t>(result), mem);

    cpu.PC += 1;
    if (regVal == 6)
        cpu.cycles += 12;
    else
        cpu.cycles += 4;
}

void ld_r8_imm8(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t regVal = (opcode >> 3) & 7;
    uint8_t value = mem.read(cpu.PC + 1);

    cpu.setRegister8(regVal, value, mem);

    cpu.PC += 2;
    if (regVal == 6)
        cpu.cycles += 12;
    else
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

    for (uint8_t i = 0x04; i <= 0x3C; i += 0x08)
    {
        opcode_table[i] = inc_r8;
    }

    for (uint8_t i = 0x05; i <= 0x3D; i += 0x08)
    {
        opcode_table[i] = dec_r8;
    }
    for (uint8_t i = 5; i <= 0x3E; i += 0x08)
    {
        opcode_table[i] = ld_r8_imm8;
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
