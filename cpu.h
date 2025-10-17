#include <cstdint>
#include "memory.h"

struct CPU
{
    union
    {
        struct
        {
            uint8_t F, A;
        };
        uint16_t AF; // Little Endian
    };
    union
    {
        struct
        {
            uint8_t C, B;
        };
        uint16_t BC;
    };
    union
    {
        struct
        {
            uint8_t E, D;
        };
        uint16_t DE;
    };
    union
    {
        struct
        {
            uint8_t L, H;
        };
        uint16_t HL;
    };

    uint16_t SP; // Stack Pointer
    uint16_t PC; // Program Counter/Pointer

    void reset();
    void step(Memory &memory);

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
