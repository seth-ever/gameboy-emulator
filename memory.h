#ifndef MEM_H
#define MEM_H

#include <cstdint>
#include <cstring>

struct Memory
{
    uint8_t data[0x10000]; // 64 KB Memory

    Memory() { memset(data, 0, sizeof(data)); }

    uint8_t read(uint16_t addr)
    {
        // optional: check ranges for I/O, VRAM, etc.
        return data[addr];
    }

    void write(uint16_t addr, uint8_t value)
    {
        // optional: block writes to ROM, handle hardware
        data[addr] = value;
    }

    // Read a 16-bit word (little endian)
    uint16_t readWord(uint16_t addr) const
    {
        return data[addr] | (data[addr + 1] << 8);
    }

    // Write a 16-bit word (little endian)
    void writeWord(uint16_t addr, uint16_t value)
    {
        data[addr] = value & 0xFF;
        data[addr + 1] = (value >> 8) & 0xFF;
    }
};

#endif