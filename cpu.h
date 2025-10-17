#include <cstdint>

struct Memory
{
    uint8_t data[0x10000]; // 64 KB Memory

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
};

struct CPU
{
    union
    {
        struct
        {
            uint8_t F; // Flags
            uint8_t A; // Accumulator
        };
        uint16_t AF; // Combined 16-bit register
    };

    union
    {
        struct
        {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };

    union
    {
        struct
        {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };

    union
    {
        struct
        {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };

    uint16_t SP; // Stack Pointer
    uint16_t PC; // Program Counter/Pointer

    void reset();
    void step(uint8_t *memory);

    // Flag inline functions
    inline bool getZero() { return F & 0x80; }      // Bit 7
    inline bool getSubtract() { return F & 0x40; }  // Bit 6
    inline bool getHalfCarry() { return F & 0x20; } // Bit 5
    inline bool getCarry() { return F & 0x10; }     // Bit 4

    inline void setZero(bool v) { F = v ? (F | 0x80) : (F & ~0x80); }
    inline void setSubtract(bool v) { F = v ? (F | 0x40) : (F & ~0x40); }
    inline void setHalfCarry(bool v) { F = v ? (F | 0x20) : (F & ~0x20); }
    inline void setCarry(bool v) { F = v ? (F | 0x10) : (F & ~0x10); }
};
