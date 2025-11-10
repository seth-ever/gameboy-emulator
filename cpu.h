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

    uint16_t SP;         // Stack Pointer
    uint16_t PC;         // Program Counter/Pointer
    uint64_t cycles = 0; // Cycle counts since reset, for syncing processes

    void
    reset();
    void step(Memory &memory);

    uint8_t getRegister8(uint8_t regVal, Memory &mem)
    {
        switch (regVal)
        {
        case 0:
            return B;
        case 1:
            return C;
        case 2:
            return D;
        case 3:
            return E;
        case 4:
            return H;
        case 5:
            return L;
        case 6:
            return mem.read(HL); // [HL]
        case 7:
            return A;
        default:
            return 0;
        }
    }

    void setRegister8(uint8_t regVal, uint8_t value, Memory &mem)
    {
        switch (regVal)
        {
        case 0:
            B = value;
            break;
        case 1:
            C = value;
            break;
        case 2:
            D = value;
            break;
        case 3:
            E = value;
            break;
        case 4:
            H = value;
            break;
        case 5:
            L = value;
            break;
        case 6:
            mem.write(HL, value);
            break; // (HL)
        case 7:
            A = value;
            break;
        }
    }

    uint16_t getRegister16(uint8_t regVal)
    {
        switch (regVal)
        {
        case 0:
            return BC;
        case 1:
            return DE;
        case 2:
            return HL;
        case 3:
            return SP;
        default:
            return 0;
        }
    }

    uint16_t getRegister16mem(uint8_t regVal)
    {
        switch (regVal)
        {
        case 0:
            return BC;
        case 1:
            return DE;
        case 2:
            return HL; // HL++
        case 3:
            return HL; // HL--
        default:
            return 0;
        }
    }

    void setRegister16(uint8_t regVal, uint16_t value)
    {
        switch (regVal)
        {
        case 0:
            BC = value;
            break;
        case 1:
            DE = value;
            break;
        case 2:
            HL = value;
            break;
        case 3:
            SP = value;
            break;
        }
    }

    // Flag inline functions
    inline bool getZero() { return F & 0x80; }      // Bit 7
    inline bool getSubtract() { return F & 0x40; }  // Bit 6
    inline bool getHalfCarry() { return F & 0x20; } // Bit 5
    /*
        16 bit add: ((cpu.HL & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF)
        8 bit increment: (value & 0x0F) + 1 > 0x0F
        8 bit decrement: (value & 0x0F) == 0x00)
    */
    inline bool getCarry() { return F & 0x10; } // Bit 4

    inline void setZero(bool v) { F = v ? (F | 0x80) : (F & ~0x80); }
    inline void setSubtract(bool v) { F = v ? (F | 0x40) : (F & ~0x40); }
    inline void setHalfCarry(bool v) { F = v ? (F | 0x20) : (F & ~0x20); }
    inline void setCarry(bool v) { F = v ? (F | 0x10) : (F & ~0x10); }
};
