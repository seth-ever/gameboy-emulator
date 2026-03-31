#include "cpu.h"

Memory memory;
CPU cpu;

/*
ld_destination_source

rX: register of size X bits.
rXmem: location in memory of size X bits
immX: the next X bits in memory after opcode

*/

typedef void (*Instruction)(CPU &, Memory &, uint8_t opcode);

void unimplemented_opcode(CPU &cpu, Memory &mem, uint8_t opcode)
{
    std::cerr << "Unimplemented opcode: 0x"
              << std::hex << (int)opcode << "\n";
    std::exit(1);
}

Instruction opcode_table[256] = {unimplemented_opcode /*Defualt value for catching undefined instruction, different from INVALID*/};
Instruction CB_opcode_table[256] = {unimplemented_opcode /*Defualt value for catching undefined instruction, different from INVALID*/}; // For two-byte instructions

// CB PREFIX INSTRUCTIONS --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void rlc_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void rrc_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void rl_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void rr_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sla_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sra_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void swap_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void srl_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void bit_b3_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void res_b3_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void set_b3_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void initialize_CB_opcode_table()
{
    for (uint8_t i = 0x00; i < 0x08; i++)
        CB_opcode_table[i] = rlc_r8;

    for (uint8_t i = 0x08; i < 0x10; i++)
        CB_opcode_table[i] = rrc_r8;

    for (uint8_t i = 0x10; i < 0x18; i++)
        CB_opcode_table[i] = rl_r8;

    for (uint8_t i = 0x18; i < 0x20; i++)
        CB_opcode_table[i] = rr_r8;

    for (uint8_t i = 0x20; i < 0x28; i++)
        CB_opcode_table[i] = sla_r8;

    for (uint8_t i = 0x28; i < 0x30; i++)
        CB_opcode_table[i] = sra_r8;

    for (uint8_t i = 0x30; i < 0x38; i++)
        CB_opcode_table[i] = swap_r8;

    for (uint8_t i = 0x38; i < 0x40; i++)
        CB_opcode_table[i] = srl_r8;

    for (uint8_t i = 0x40; i < 0x80; i++)
        CB_opcode_table[i] = bit_b3_r8;

    for (uint8_t i = 0x80; i < 0xC0; i++)
        CB_opcode_table[i] = res_b3_r8;

    for (uint8_t i = 0xC0; i <= 0xFF; i++)
        CB_opcode_table[i] = set_b3_r8;
}

// BASE OPCODE UNSTRUCTIONS ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// BLOCK 0
void NOP(CPU &cpu, Memory &mem, uint8_t opcode)
{
    cpu.PC += 1;
    cpu.cycles += 4;
}

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

void rlca(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t old_b7 = (cpu.A >> 7) & 1;

    cpu.A = (uint8_t)((cpu.A << 1) | (cpu.A >> 7));
    cpu.setCarry(old_b7);

    cpu.PC += 1;
    cpu.cycles += 4;
}

void rrca(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t old_b1 = cpu.A & 1;

    cpu.A = (uint8_t)((cpu.A >> 1) | (cpu.A << 7));
    cpu.setCarry(old_b1);

    cpu.PC += 1;
    cpu.cycles += 4;
}

void rla(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t old_b7 = (cpu.A >> 7) & 1;
    uint8_t old_carry = cpu.getCarry();

    cpu.A = (uint8_t)((cpu.A << 1) | old_carry);
    cpu.setCarry(old_b7);

    cpu.PC += 1;
    cpu.cycles += 4;
}

void rra(CPU &cpu, Memory &mem, uint8_t opcode)
{
    uint8_t old_b0 = cpu.A & 1;
    uint8_t old_carry = cpu.getCarry();

    cpu.A = (uint8_t)((cpu.A >> 1) | (old_carry << 7));

    cpu.PC += 1;
    cpu.cycles += 4;
}

void daa(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void cpl(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void scf(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ccf(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void jr_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void jr_cond_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void stop(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

// BLOCK 1 8-BIT REGISTER-TO-REGISTER LOADS
void ld_r8_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}
void halt(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

// BLOCK 2 8-BIT ARITHMETIC
void add_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void adc_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sub_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sbc_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void and_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void xor_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void or_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void cp_a_r8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

// BLOCK 3
void add_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void adc_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sub_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void sbc_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void and_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void xor_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void or_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void cp_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ret_cond(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ret(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void reti(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void jp_cond_imm16(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void jp_imm16(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void jp_hl(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void call_cond_imm16(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void call_imm16(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void rst_tgt3(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void pop_r16stk(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void push_r16stk(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void CB_prefix(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // two byte instructions, with CB as the first byte
    //  TO BE IMPLEMENTED
}

void ldh_c_a(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ldh_imm8_a(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ld_imm16_a(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ldh_a_c(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ldh_a_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ld_a_imm16(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void add_sp_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ld_hl_sp_PLUS_imm8(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ld_sp_hl(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void di(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void ei(CPU &cpu, Memory &mem, uint8_t opcode) // TO BE IMPLEMENTED ----------------------------------------------------------------------------------------------
{
    // TO BE IMPLEMENTED
}

void INVALID(CPU &cpu, Memory &mem, uint8_t opcode)
{
    cpu.locked = true;
}

void initialize_opcode_table()
{
    // BLOCK 0
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

    for (uint8_t i = 0x04; i <= 0x3C; i += 0x08) // inc_r8
    {
        opcode_table[i] = inc_r8;
    }

    for (uint8_t i = 0x05; i <= 0x3D; i += 0x08) // dec_r8
    {
        opcode_table[i] = dec_r8;
    }

    for (uint8_t i = 0x06; i <= 0x3E; i += 0x08) // ld_r8_imm8
    {
        opcode_table[i] = ld_r8_imm8;
    }

    opcode_table[0x07] = rlca;
    opcode_table[0x0F] = rrca;
    opcode_table[0x17] = rla;
    opcode_table[0x1F] = rra;
    opcode_table[0x27] = daa;
    opcode_table[0x2F] = cpl;
    opcode_table[0x37] = scf;
    opcode_table[0x3F] = ccf;

    opcode_table[0x18] = jr_imm8;
    for (uint8_t i = 0x20; i < 0x40; i += 0x08)
        opcode_table[i] = jr_cond_imm8;

    opcode_table[0x10] = stop;

    // BLOCK 1
    for (uint8_t i = 0x40; i < 0x80; i++)
        opcode_table[i] = ld_r8_r8;

    // halt exception, when trying to encode ld [hl] [hl]
    opcode_table[0x75] = halt;

    // BLOCK 2
    for (uint8_t i = 0x80; i < 0x88; i++)
        opcode_table[i] = add_a_r8;

    for (uint8_t i = 0x88; i < 0x90; i++)
        opcode_table[i] = adc_a_r8;

    for (uint8_t i = 0x90; i < 0x98; i++)
        opcode_table[i] = sub_a_r8;

    for (uint8_t i = 0x98; i < 0xA0; i++)
        opcode_table[i] = sbc_a_r8;

    for (uint8_t i = 0xA0; i < 0xA8; i++)
        opcode_table[i] = and_a_r8;

    for (uint8_t i = 0xA8; i < 0xB0; i++)
        opcode_table[i] = xor_a_r8;

    for (uint8_t i = 0xB0; i < 0xB8; i++)
        opcode_table[i] = or_a_r8;

    for (uint8_t i = 0xB8; i < 0xC0; i++)
        opcode_table[i] = cp_a_r8;

    // BLOCK 3
    opcode_table[0xC6] = add_a_imm8;
    opcode_table[0xCE] = adc_a_imm8;
    opcode_table[0xD6] = sub_a_imm8;
    opcode_table[0xDE] = sbc_a_imm8;
    opcode_table[0xE6] = and_a_imm8;
    opcode_table[0xEE] = xor_a_imm8;
    opcode_table[0xF6] = or_a_imm8;
    opcode_table[0xFE] = cp_a_imm8;

    for (uint8_t i = 0xC0; i < 0xE0; i += 0x08)
        opcode_table[i] = ret_cond;

    opcode_table[0xC9] = ret;
    opcode_table[0xD9] = reti;

    for (uint8_t i = 0xC2; i < 0xE2; i += 0x08)
        opcode_table[i] = jp_cond_imm16;

    opcode_table[0xC3] = jp_imm16;
    opcode_table[0xE9] = jp_hl;

    for (uint8_t i = 0xC4; i < 0xE4; i += 0x08)
        opcode_table[i] = call_cond_imm16;

    opcode_table[0xCD] = call_imm16;

    for (uint8_t i = 0xC7; i <= 0xFF; i += 0x08)
        opcode_table[i] = rst_tgt3;

    for (uint8_t i = 0xC1; i <= 0xF1; i += 0x10)
        opcode_table[i] = pop_r16stk;

    for (uint8_t i = 0xC5; i <= 0xF5; i += 0x10)
        opcode_table[i] = push_r16stk;

    opcode_table[0xCB] = CB_prefix;

    opcode_table[0xE2] = ldh_c_a;
    opcode_table[0xE0] = ldh_imm8_a;
    opcode_table[0xEA] = ld_imm16_a;
    opcode_table[0xF2] = ldh_a_c;
    opcode_table[0xF0] = ldh_a_imm8;
    opcode_table[0xFA] = ld_a_imm16;

    opcode_table[0xE8] = add_sp_imm8;
    opcode_table[0xF8] = ld_hl_sp_PLUS_imm8;
    opcode_table[0xF9] = ld_sp_hl;

    opcode_table[0xF3] = di;
    opcode_table[0xFB] = ei;

    size_t invalid_addresses[11] = {0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD};
    for (auto i : invalid_addresses)
        opcode_table[i] = INVALID;
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
    if (locked)
        return;

    uint8_t opcode = memory.read(cpu.PC);
    opcode_table[opcode](*this, memory, opcode);
}
