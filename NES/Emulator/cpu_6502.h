#pragma once
#include <stdint.h>
#include <memory>
#include <condition_variable>
#include "Memory/Mappers/mapper.h"


struct ProcessorStatus {
    uint8_t N = 0;
    uint8_t V = 0;
    uint8_t R = 1;
    uint8_t B = 0;
    uint8_t D = 0;
    uint8_t I = 0;
    uint8_t Z = 0;
    uint8_t C = 0;

    uint8_t asByte() {
        return (N << 7) | (V << 6) | (R << 5) | (B << 4) | (D << 3) | (I << 2) | (Z << 1) | (C << 0);
    }

    void fromByte(uint8_t status) {
        C = (status >> 0) & 0x01;
        Z = (status >> 1) & 0x01;
        I = (status >> 2) & 0x01;
        D = (status >> 3) & 0x01;
        B = (status >> 4) & 0x01;
        R = (status >> 5) & 0x01;
        V = (status >> 6) & 0x01;
        N = (status >> 7) & 0x01;
    }
};

class CPU_6502
{
public:
    CPU_6502(std::shared_ptr<Mapper> mapper, std::shared_ptr<std::condition_variable> clock);
    ~CPU_6502();

    void next();
    void continueExec();
    void SetIRQ();
    void ClearIRQ();
    void SetNMI();
    void ClearNMI();
    void SetRESET();
    void ClearRESET();

private:
    std::shared_ptr<std::condition_variable> clock;
    std::shared_ptr<Mapper> mapper;
    std::mutex mu;

    uint16_t        PC = 0x00;
    uint8_t         SP = 0xFF;
    ProcessorStatus P;
    uint8_t         A = 0x00;
    uint8_t         X = 0x00;
    uint8_t         Y = 0x00;

    bool IRQ_set = false;
    bool NMI_set = false;
    bool RESET_set = false;

    void processInterrupts();

    inline void push8(uint8_t value);
    inline void push16(uint16_t value);
    inline uint8_t pull8();
    inline uint16_t pull16();
    inline uint8_t read8(uint16_t address);
    inline uint16_t read16(uint16_t address);
    inline void write8(uint16_t address, uint8_t value);
    inline void write16(uint16_t address, uint16_t value);

    inline void pushProcessorStatus();
    inline void pullProcessorStatus();

    inline void tick();

    void IRQ();
    void NMI();
    void RESET();

    inline uint16_t ABS_Address_R();
    inline uint16_t ABS_Address_RW();
    inline uint16_t ABS_Address_W();
    inline uint16_t ZPA_Address_R();
    inline uint16_t ZPA_Address_RW();
    inline uint16_t ZPA_Address_W();
    inline uint16_t ZPX_Address_R();
    inline uint16_t ZPX_Address_RW();
    inline uint16_t ZPX_Address_W();
    inline uint16_t ZPY_Address_R();
    inline uint16_t ZPY_Address_RW();
    inline uint16_t ZPY_Address_W();
    inline uint16_t ABX_Address_R();
    inline uint16_t ABX_Address_RW();
    inline uint16_t ABX_Address_W();
    inline uint16_t ABY_Address_R();
    inline uint16_t ABY_Address_RW();
    inline uint16_t ABY_Address_W();
    inline uint16_t INX_Address_R();
    inline uint16_t INX_Address_RW();
    inline uint16_t INX_Address_W();
    inline uint16_t INY_Address_R();
    inline uint16_t INY_Address_RW();
    inline uint16_t INY_Address_W();

    inline void    ADC(uint8_t op      );	//done
    inline void    AND(uint8_t op      );	//done
    inline uint8_t ASL(uint8_t op      );	//done
    inline void    BCC(                );	//done
    inline void    BCS(                );	//done
    inline void    BEQ(                );	//done
    inline void    BIT(uint8_t op      );	//done
    inline void    BMI(                );	//done
    inline void    BNE(                );	//done
    inline void    BPL(                );	//done
    inline void    BRK(                );	//done
    inline void    BVC(                );	//done
    inline void    BVS(                );	//done
    inline void    CLC(                );	//done
    inline void    CLD(                );	//done
    inline void    CLI(                );	//done
    inline void    CLV(                );	//done
    inline void    CMP(uint8_t op      );	//done
    inline void    CPX(uint8_t op      );	//done
    inline void    CPY(uint8_t op      );	//done
    inline uint8_t DEC(uint8_t op      );	//done
    inline void    DEX(                );	//done
    inline void    DEY(                );	//done
    inline void    EOR(uint8_t op      );	//done
    inline uint8_t INC(uint8_t op      );	//done
    inline void    INX(                );	//done
    inline void    INY(                );	//done
    inline void    JMP(uint16_t address);	//done
    inline void    JSR(                );	//done
    inline void    LDA(uint8_t op      );	//done
    inline void    LDX(uint8_t op      );	//done
    inline void    LDY(uint8_t op      );	//done
    inline uint8_t LSR(uint8_t op      );	//done
    inline void    NOP(                );	//done
    inline void    ORA(uint8_t op      );	//done
    inline void    PHA(                );	//done
    inline void    PHP(                );	//done
    inline void    PLA(                );	//done
    inline void    PLP(                );	//done
    inline uint8_t ROL(uint8_t op      );	//done
    inline uint8_t ROR(uint8_t op      );	//done
    inline void    RTI(                );	//done
    inline void    RTS(                );	//done
    inline void    SBC(uint8_t op      );	//done
    inline void    SEC(                );	//done
    inline void    SED(                );	//done
    inline void    SEI(                );	//done
    inline uint8_t STA(                );	//done
    inline uint8_t STX(                );	//done
    inline uint8_t STY(                );	//done
    inline void    TAX(                );	//done
    inline void    TAY(                );	//done
    inline void    TSX(                );	//done
    inline void    TXA(                );	//done
    inline void    TXS(                );	//done
    inline void    TYA(                );	//done

    void op0x00();
    void op0x01();
    void op0x02();
    void op0x03();
    void op0x04();
    void op0x05();
    void op0x06();
    void op0x07();
    void op0x08();
    void op0x09();
    void op0x0A();
    void op0x0B();
    void op0x0C();
    void op0x0D();
    void op0x0E();
    void op0x0F();
    void op0x10();
    void op0x11();
    void op0x12();
    void op0x13();
    void op0x14();
    void op0x15();
    void op0x16();
    void op0x17();
    void op0x18();
    void op0x19();
    void op0x1A();
    void op0x1B();
    void op0x1C();
    void op0x1D();
    void op0x1E();
    void op0x1F();
    void op0x20();
    void op0x21();
    void op0x22();
    void op0x23();
    void op0x24();
    void op0x25();
    void op0x26();
    void op0x27();
    void op0x28();
    void op0x29();
    void op0x2A();
    void op0x2B();
    void op0x2C();
    void op0x2D();
    void op0x2E();
    void op0x2F();
    void op0x30();
    void op0x31();
    void op0x32();
    void op0x33();
    void op0x34();
    void op0x35();
    void op0x36();
    void op0x37();
    void op0x38();
    void op0x39();
    void op0x3A();
    void op0x3B();
    void op0x3C();
    void op0x3D();
    void op0x3E();
    void op0x3F();
    void op0x40();
    void op0x41();
    void op0x42();
    void op0x43();
    void op0x44();
    void op0x45();
    void op0x46();
    void op0x47();
    void op0x48();
    void op0x49();
    void op0x4A();
    void op0x4B();
    void op0x4C();
    void op0x4D();
    void op0x4E();
    void op0x4F();
    void op0x50();
    void op0x51();
    void op0x52();
    void op0x53();
    void op0x54();
    void op0x55();
    void op0x56();
    void op0x57();
    void op0x58();
    void op0x59();
    void op0x5A();
    void op0x5B();
    void op0x5C();
    void op0x5D();
    void op0x5E();
    void op0x5F();
    void op0x60();
    void op0x61();
    void op0x62();
    void op0x63();
    void op0x64();
    void op0x65();
    void op0x66();
    void op0x67();
    void op0x68();
    void op0x69();
    void op0x6A();
    void op0x6B();
    void op0x6C();
    void op0x6D();
    void op0x6E();
    void op0x6F();
    void op0x70();
    void op0x71();
    void op0x72();
    void op0x73();
    void op0x74();
    void op0x75();
    void op0x76();
    void op0x77();
    void op0x78();
    void op0x79();
    void op0x7A();
    void op0x7B();
    void op0x7C();
    void op0x7D();
    void op0x7E();
    void op0x7F();
    void op0x80();
    void op0x81();
    void op0x82();
    void op0x83();
    void op0x84();
    void op0x85();
    void op0x86();
    void op0x87();
    void op0x88();
    void op0x89();
    void op0x8A();
    void op0x8B();
    void op0x8C();
    void op0x8D();
    void op0x8E();
    void op0x8F();
    void op0x90();
    void op0x91();
    void op0x92();
    void op0x93();
    void op0x94();
    void op0x95();
    void op0x96();
    void op0x97();
    void op0x98();
    void op0x99();
    void op0x9A();
    void op0x9B();
    void op0x9C();
    void op0x9D();
    void op0x9E();
    void op0x9F();
    void op0xA0();
    void op0xA1();
    void op0xA2();
    void op0xA3();
    void op0xA4();
    void op0xA5();
    void op0xA6();
    void op0xA7();
    void op0xA8();
    void op0xA9();
    void op0xAA();
    void op0xAB();
    void op0xAC();
    void op0xAD();
    void op0xAE();
    void op0xAF();
    void op0xB0();
    void op0xB1();
    void op0xB2();
    void op0xB3();
    void op0xB4();
    void op0xB5();
    void op0xB6();
    void op0xB7();
    void op0xB8();
    void op0xB9();
    void op0xBA();
    void op0xBB();
    void op0xBC();
    void op0xBD();
    void op0xBE();
    void op0xBF();
    void op0xC0();
    void op0xC1();
    void op0xC2();
    void op0xC3();
    void op0xC4();
    void op0xC5();
    void op0xC6();
    void op0xC7();
    void op0xC8();
    void op0xC9();
    void op0xCA();
    void op0xCB();
    void op0xCC();
    void op0xCD();
    void op0xCE();
    void op0xCF();
    void op0xD0();
    void op0xD1();
    void op0xD2();
    void op0xD3();
    void op0xD4();
    void op0xD5();
    void op0xD6();
    void op0xD7();
    void op0xD8();
    void op0xD9();
    void op0xDA();
    void op0xDB();
    void op0xDC();
    void op0xDD();
    void op0xDE();
    void op0xDF();
    void op0xE0();
    void op0xE1();
    void op0xE2();
    void op0xE3();
    void op0xE4();
    void op0xE5();
    void op0xE6();
    void op0xE7();
    void op0xE8();
    void op0xE9();
    void op0xEA();
    void op0xEB();
    void op0xEC();
    void op0xED();
    void op0xEE();
    void op0xEF();
    void op0xF0();
    void op0xF1();
    void op0xF2();
    void op0xF3();
    void op0xF4();
    void op0xF5();
    void op0xF6();
    void op0xF7();
    void op0xF8();
    void op0xF9();
    void op0xFA();
    void op0xFB();
    void op0xFC();
    void op0xFD();
    void op0xFE();
    void op0xFF();

    inline void NOOP();

    void (CPU_6502::*opTable[0x100])() =
    {
        &CPU_6502::op0x00,
        &CPU_6502::op0x01,
        &CPU_6502::op0x02,
        &CPU_6502::op0x03,
        &CPU_6502::op0x04,
        &CPU_6502::op0x05,
        &CPU_6502::op0x06,
        &CPU_6502::op0x07,
        &CPU_6502::op0x08,
        &CPU_6502::op0x09,
        &CPU_6502::op0x0A,
        &CPU_6502::op0x0B,
        &CPU_6502::op0x0C,
        &CPU_6502::op0x0D,
        &CPU_6502::op0x0E,
        &CPU_6502::op0x0F,
        &CPU_6502::op0x10,
        &CPU_6502::op0x11,
        &CPU_6502::op0x12,
        &CPU_6502::op0x13,
        &CPU_6502::op0x14,
        &CPU_6502::op0x15,
        &CPU_6502::op0x16,
        &CPU_6502::op0x17,
        &CPU_6502::op0x18,
        &CPU_6502::op0x19,
        &CPU_6502::op0x1A,
        &CPU_6502::op0x1B,
        &CPU_6502::op0x1C,
        &CPU_6502::op0x1D,
        &CPU_6502::op0x1E,
        &CPU_6502::op0x1F,
        &CPU_6502::op0x20,
        &CPU_6502::op0x21,
        &CPU_6502::op0x22,
        &CPU_6502::op0x23,
        &CPU_6502::op0x24,
        &CPU_6502::op0x25,
        &CPU_6502::op0x26,
        &CPU_6502::op0x27,
        &CPU_6502::op0x28,
        &CPU_6502::op0x29,
        &CPU_6502::op0x2A,
        &CPU_6502::op0x2B,
        &CPU_6502::op0x2C,
        &CPU_6502::op0x2D,
        &CPU_6502::op0x2E,
        &CPU_6502::op0x2F,
        &CPU_6502::op0x30,
        &CPU_6502::op0x31,
        &CPU_6502::op0x32,
        &CPU_6502::op0x33,
        &CPU_6502::op0x34,
        &CPU_6502::op0x35,
        &CPU_6502::op0x36,
        &CPU_6502::op0x37,
        &CPU_6502::op0x38,
        &CPU_6502::op0x39,
        &CPU_6502::op0x3A,
        &CPU_6502::op0x3B,
        &CPU_6502::op0x3C,
        &CPU_6502::op0x3D,
        &CPU_6502::op0x3E,
        &CPU_6502::op0x3F,
        &CPU_6502::op0x40,
        &CPU_6502::op0x41,
        &CPU_6502::op0x42,
        &CPU_6502::op0x43,
        &CPU_6502::op0x44,
        &CPU_6502::op0x45,
        &CPU_6502::op0x46,
        &CPU_6502::op0x47,
        &CPU_6502::op0x48,
        &CPU_6502::op0x49,
        &CPU_6502::op0x4A,
        &CPU_6502::op0x4B,
        &CPU_6502::op0x4C,
        &CPU_6502::op0x4D,
        &CPU_6502::op0x4E,
        &CPU_6502::op0x4F,
        &CPU_6502::op0x50,
        &CPU_6502::op0x51,
        &CPU_6502::op0x52,
        &CPU_6502::op0x53,
        &CPU_6502::op0x54,
        &CPU_6502::op0x55,
        &CPU_6502::op0x56,
        &CPU_6502::op0x57,
        &CPU_6502::op0x58,
        &CPU_6502::op0x59,
        &CPU_6502::op0x5A,
        &CPU_6502::op0x5B,
        &CPU_6502::op0x5C,
        &CPU_6502::op0x5D,
        &CPU_6502::op0x5E,
        &CPU_6502::op0x5F,
        &CPU_6502::op0x60,
        &CPU_6502::op0x61,
        &CPU_6502::op0x62,
        &CPU_6502::op0x63,
        &CPU_6502::op0x64,
        &CPU_6502::op0x65,
        &CPU_6502::op0x66,
        &CPU_6502::op0x67,
        &CPU_6502::op0x68,
        &CPU_6502::op0x69,
        &CPU_6502::op0x6A,
        &CPU_6502::op0x6B,
        &CPU_6502::op0x6C,
        &CPU_6502::op0x6D,
        &CPU_6502::op0x6E,
        &CPU_6502::op0x6F,
        &CPU_6502::op0x70,
        &CPU_6502::op0x71,
        &CPU_6502::op0x72,
        &CPU_6502::op0x73,
        &CPU_6502::op0x74,
        &CPU_6502::op0x75,
        &CPU_6502::op0x76,
        &CPU_6502::op0x77,
        &CPU_6502::op0x78,
        &CPU_6502::op0x79,
        &CPU_6502::op0x7A,
        &CPU_6502::op0x7B,
        &CPU_6502::op0x7C,
        &CPU_6502::op0x7D,
        &CPU_6502::op0x7E,
        &CPU_6502::op0x7F,
        &CPU_6502::op0x80,
        &CPU_6502::op0x81,
        &CPU_6502::op0x82,
        &CPU_6502::op0x83,
        &CPU_6502::op0x84,
        &CPU_6502::op0x85,
        &CPU_6502::op0x86,
        &CPU_6502::op0x87,
        &CPU_6502::op0x88,
        &CPU_6502::op0x89,
        &CPU_6502::op0x8A,
        &CPU_6502::op0x8B,
        &CPU_6502::op0x8C,
        &CPU_6502::op0x8D,
        &CPU_6502::op0x8E,
        &CPU_6502::op0x8F,
        &CPU_6502::op0x90,
        &CPU_6502::op0x91,
        &CPU_6502::op0x92,
        &CPU_6502::op0x93,
        &CPU_6502::op0x94,
        &CPU_6502::op0x95,
        &CPU_6502::op0x96,
        &CPU_6502::op0x97,
        &CPU_6502::op0x98,
        &CPU_6502::op0x99,
        &CPU_6502::op0x9A,
        &CPU_6502::op0x9B,
        &CPU_6502::op0x9C,
        &CPU_6502::op0x9D,
        &CPU_6502::op0x9E,
        &CPU_6502::op0x9F,
        &CPU_6502::op0xA0,
        &CPU_6502::op0xA1,
        &CPU_6502::op0xA2,
        &CPU_6502::op0xA3,
        &CPU_6502::op0xA4,
        &CPU_6502::op0xA5,
        &CPU_6502::op0xA6,
        &CPU_6502::op0xA7,
        &CPU_6502::op0xA8,
        &CPU_6502::op0xA9,
        &CPU_6502::op0xAA,
        &CPU_6502::op0xAB,
        &CPU_6502::op0xAC,
        &CPU_6502::op0xAD,
        &CPU_6502::op0xAE,
        &CPU_6502::op0xAF,
        &CPU_6502::op0xB0,
        &CPU_6502::op0xB1,
        &CPU_6502::op0xB2,
        &CPU_6502::op0xB3,
        &CPU_6502::op0xB4,
        &CPU_6502::op0xB5,
        &CPU_6502::op0xB6,
        &CPU_6502::op0xB7,
        &CPU_6502::op0xB8,
        &CPU_6502::op0xB9,
        &CPU_6502::op0xBA,
        &CPU_6502::op0xBB,
        &CPU_6502::op0xBC,
        &CPU_6502::op0xBD,
        &CPU_6502::op0xBE,
        &CPU_6502::op0xBF,
        &CPU_6502::op0xC0,
        &CPU_6502::op0xC1,
        &CPU_6502::op0xC2,
        &CPU_6502::op0xC3,
        &CPU_6502::op0xC4,
        &CPU_6502::op0xC5,
        &CPU_6502::op0xC6,
        &CPU_6502::op0xC7,
        &CPU_6502::op0xC8,
        &CPU_6502::op0xC9,
        &CPU_6502::op0xCA,
        &CPU_6502::op0xCB,
        &CPU_6502::op0xCC,
        &CPU_6502::op0xCD,
        &CPU_6502::op0xCE,
        &CPU_6502::op0xCF,
        &CPU_6502::op0xD0,
        &CPU_6502::op0xD1,
        &CPU_6502::op0xD2,
        &CPU_6502::op0xD3,
        &CPU_6502::op0xD4,
        &CPU_6502::op0xD5,
        &CPU_6502::op0xD6,
        &CPU_6502::op0xD7,
        &CPU_6502::op0xD8,
        &CPU_6502::op0xD9,
        &CPU_6502::op0xDA,
        &CPU_6502::op0xDB,
        &CPU_6502::op0xDC,
        &CPU_6502::op0xDD,
        &CPU_6502::op0xDE,
        &CPU_6502::op0xDF,
        &CPU_6502::op0xE0,
        &CPU_6502::op0xE1,
        &CPU_6502::op0xE2,
        &CPU_6502::op0xE3,
        &CPU_6502::op0xE4,
        &CPU_6502::op0xE5,
        &CPU_6502::op0xE6,
        &CPU_6502::op0xE7,
        &CPU_6502::op0xE8,
        &CPU_6502::op0xE9,
        &CPU_6502::op0xEA,
        &CPU_6502::op0xEB,
        &CPU_6502::op0xEC,
        &CPU_6502::op0xED,
        &CPU_6502::op0xEE,
        &CPU_6502::op0xEF,
        &CPU_6502::op0xF0,
        &CPU_6502::op0xF1,
        &CPU_6502::op0xF2,
        &CPU_6502::op0xF3,
        &CPU_6502::op0xF4,
        &CPU_6502::op0xF5,
        &CPU_6502::op0xF6,
        &CPU_6502::op0xF7,
        &CPU_6502::op0xF8,
        &CPU_6502::op0xF9,
        &CPU_6502::op0xFA,
        &CPU_6502::op0xFB,
        &CPU_6502::op0xFC,
        &CPU_6502::op0xFD,
        &CPU_6502::op0xFE,
        &CPU_6502::op0xFF
    };
};

inline uint8_t bit(uint8_t value, int index);
inline uint16_t bit(uint16_t value, int index);
