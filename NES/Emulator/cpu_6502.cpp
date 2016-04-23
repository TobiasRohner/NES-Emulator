#include "cpu_6502.h"
#include <iostream>
#include <iomanip>
#include <string>
#include "options.h"

void terminateOpcode(std::shared_ptr<COMMUNCATION_BRIDGE> bridge) {
    while (!bridge.get()->OPCODE_TERMINATED) { bridge.get()->clk.notify_all(); }
}

CPU_6502::CPU_6502(std::shared_ptr<Mapper> mapper, std::shared_ptr<COMMUNCATION_BRIDGE> bridge)
{
    this->bridge = bridge;
    this->mapper = mapper;
}


CPU_6502::~CPU_6502()
{
}

void CPU_6502::processOpcode()
{
    uint8_t opcode = read8(PC);
#ifdef DEBUG
    std::cout << "Memory Location 0x";
    std::cout << std::hex << std::uppercase << PC << std::nouppercase << std::dec;
    std::cout << "\tOpcode: 0x";
    std::cout << std::hex << std::uppercase << (uint16_t)opcode << std::nouppercase << std::dec;
    std::cout << std::endl;
#endif
    PC++;
    tick();
    (this->*opTable[opcode])();
}

void CPU_6502::next()
{
    bridge.get()->OPCODE_TERMINATED = false;
    processOpcode();
    processInterrupts();
    bridge.get()->OPCODE_TERMINATED = true;
}

void CPU_6502::SetIRQ()
{
    IRQ_set = true;
}

void CPU_6502::ClearIRQ()
{
    IRQ_set = false;
}

void CPU_6502::SetNMI()
{
    NMI_set = true;
}

void CPU_6502::ClearNMI()
{
    NMI_set = false;
}

void CPU_6502::SetRESET()
{
    RESET_set = true;
}

void CPU_6502::ClearRESET()
{
    RESET_set = false;
}

uint16_t CPU_6502::getPC() {
    return PC;
}

uint8_t CPU_6502::getSP() {
    return SP;
}

uint8_t CPU_6502::getA() {
    return A;
}

uint8_t CPU_6502::getX() {
    return X;
}

uint8_t CPU_6502::getY() {
    return Y;
}

void CPU_6502::processInterrupts()
{
    if (IRQ_set) { IRQ(); }
    if (NMI_set) { NMI(); }
    if (RESET_set) { RESET(); }
}

inline void CPU_6502::push8(uint8_t value)
{
    mapper.get()->storeCPU(0x0100 + SP--, value);
}

inline void CPU_6502::push16(uint16_t value)
{
    mapper.get()->storeCPU(0x0100 + SP--, (uint8_t)(value >> 8));
    mapper.get()->storeCPU(0x0100 + SP--, (uint8_t)(value & 0x00FF));
}

inline uint8_t CPU_6502::pull8()
{
    return mapper.get()->fetchCPU(++SP + 0x0100);
}

inline uint16_t CPU_6502::pull16()
{
    uint16_t lowByte = (uint16_t)mapper.get()->fetchCPU(++SP + 0x0100);
    return lowByte | ((uint16_t)(mapper.get()->fetchCPU(++SP + 0x0100)) << 8);
}

inline uint8_t CPU_6502::read8(uint16_t address)
{
    return mapper.get()->fetchCPU(address);
}

inline uint16_t CPU_6502::read16(uint16_t address)
{
    uint16_t lowByte = (uint16_t)(mapper.get()->fetchCPU(address));
    return ((uint16_t)(mapper.get()->fetchCPU(address + 1)) << 8) | lowByte;
}

inline void CPU_6502::write8(uint16_t address, uint8_t value)
{
    mapper.get()->storeCPU(address, value);
}

inline void CPU_6502::write16(uint16_t address, uint16_t value)
{
    mapper.get()->storeCPU(address, (uint8_t)(value & 0x00FF));
    mapper.get()->storeCPU(address, (uint8_t)(value >> 8));
}

inline void CPU_6502::pushProcessorStatus()
{
    push8(P.asByte());
}

inline void CPU_6502::pullProcessorStatus()
{
    P.fromByte(pull8());
}

inline void CPU_6502::tick()
{
    std::unique_lock<std::mutex> lock(mu);
    bridge.get()->clk.wait(lock);
}

void CPU_6502::IRQ()
{
    if (P.I == 0) {
        PC++;
        tick();
        push8((uint8_t)(PC >> 8));
        tick();
        push8((uint8_t)PC);
        tick();
        pushProcessorStatus();
        tick();
        uint16_t lowByte = (uint16_t)read8(0xFFFE);
        tick();
        uint16_t highByte = (uint16_t)read8(0xFFFF);
        PC = (highByte << 8) | lowByte;
        tick();
    }
}

void CPU_6502::NMI()
{
    PC++;
    tick();
    push8((uint8_t)(PC >> 8));
    tick();
    push8((uint8_t)PC);
    tick();
    pushProcessorStatus();
    tick();
    uint16_t lowByte = (uint16_t)read8(0xFFFA);
    tick();
    uint16_t highByte = (uint16_t)read8(0xFFFB);
    PC = (highByte << 8) | lowByte;
    tick();
}

void CPU_6502::RESET()
{
    tick();
    tick();
    tick();
    uint16_t lowByte = read8(0xFFFC);
    tick();
    uint16_t highByte = read8(0xFFFD);
    PC = (highByte << 8) | lowByte;
    tick();
}

inline uint16_t CPU_6502::ABS_Address_R()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    PC++;
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::ABS_Address_RW()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    PC++;
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::ABS_Address_W()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    PC++;
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::ZPA_Address_R()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPA_Address_RW()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPA_Address_W()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPX_Address_R()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += X;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPX_Address_RW()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += X;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPX_Address_W()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += X;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPY_Address_R()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += Y;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPY_Address_RW()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += Y;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ZPY_Address_W()
{
    uint16_t address = (uint16_t)read8(PC);
    PC++;
    tick();
    address += Y;
    address &= 0x00FF;
    tick();
    return address;
}

inline uint16_t CPU_6502::ABX_Address_R()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += X;
    PC++;
    tick();
    if (lowByte > 0x00FF) {
        tick();
    }
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::ABX_Address_RW()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += X;
    PC++;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::ABX_Address_W()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += X;
    PC++;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::ABY_Address_R()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += Y;
    PC++;
    tick();
    if (lowByte > 0x00FF) {
        tick();
    }
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::ABY_Address_RW()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += Y;
    PC++;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::ABY_Address_W()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    lowByte += Y;
    PC++;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::INX_Address_R()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    pointer += X;
    pointer &= 0x00FF;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    pointer++;
    tick();
    uint16_t highByte = (uint16_t)read8(pointer);
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::INX_Address_RW()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    pointer += X;
    pointer &= 0x00FF;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    pointer++;
    tick();
    uint16_t highByte = (uint16_t)read8(pointer);
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::INX_Address_W()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    pointer += X;
    pointer &= 0x00FF;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    pointer++;
    tick();
    uint16_t highByte = (uint16_t)read8(pointer);
    tick();
    return (highByte << 8) | lowByte;
}

inline uint16_t CPU_6502::INY_Address_R()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    tick();
    uint16_t highByte = (uint16_t)read8(pointer + 1);
    lowByte += Y;
    tick();
    if (lowByte > 0x00FF) {
        tick();
    }
    return (highByte << 8) + lowByte;

}

inline uint16_t CPU_6502::INY_Address_RW()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    tick();
    uint16_t highByte = (uint16_t)read8(pointer + 1);
    lowByte += Y;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline uint16_t CPU_6502::INY_Address_W()
{
    uint16_t pointer = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    tick();
    uint16_t highByte = (uint16_t)read8(pointer + 1);
    lowByte += Y;
    tick();
    tick();
    return (highByte << 8) + lowByte;
}

inline void CPU_6502::ADC(uint8_t op)
{
    uint16_t t = (uint16_t)A + (uint16_t)op + (uint16_t)P.C;
    P.V = bit(A, 7) != bit(t, 7);
    P.N = bit(A, 7);
    P.Z = t == 0;
    P.C = t > 0x00FF;
    A = (uint8_t)t;
    tick();
}

inline void CPU_6502::AND(uint8_t op)
{
    A &= op;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

inline uint8_t CPU_6502::ASL(uint8_t op)
{
    P.C = bit(op, 7);
    uint8_t t = op << 1;
    P.N = bit(t, 7);
    P.Z = t == 0;
    tick();
    return t;
}

inline void CPU_6502::BCC()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.C == 0) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BCS()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.C == 1) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BEQ()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.Z == 1) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BIT(uint8_t op)
{
    uint8_t t = A & op;
    P.N = bit(t, 7);
    P.V = bit(t, 6);
    P.Z = t == 0;
    tick();
}

inline void CPU_6502::BMI()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.N == 1) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BNE()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.Z == 0) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BPL()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.N == 0) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BRK()
{
    PC++;
    tick();
    push8((uint8_t)(PC >> 8));
    tick();
    push8((uint8_t)PC);
    tick();
    pushProcessorStatus();
    tick();
    uint16_t lowByte = (uint16_t)read8(0xFFFE);
    tick();
    uint16_t highByte = (uint16_t)read8(0xFFFF);
    PC = (highByte << 8) | lowByte;
    tick();
}

inline void CPU_6502::BVC()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.V == 0) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::BVS()
{
    int8_t offset = (int8_t)read8(PC);
    PC++;
    tick();
    if (P.V == 1) {
        if (((PC + offset) & 0xFF00) != (PC & 0xFF00)) {
            tick();
        }
        PC += offset;
        tick();
    }
}

inline void CPU_6502::CLC()
{
    P.C = 0;
    tick();
}

inline void CPU_6502::CLD()
{
    P.D = 0;
    tick();
}

inline void CPU_6502::CLI()
{
    P.I = 0;
    tick();
}

inline void CPU_6502::CLV()
{
    P.V = 0;
    tick();
}

inline void CPU_6502::CMP(uint8_t op)
{
    uint8_t t = A - op;
    P.N = bit(t, 7);
    P.C = A > op;
    P.Z = t == 0;
    tick();
}

inline void CPU_6502::CPX(uint8_t op)
{
    uint8_t t = X - op;
    P.N = bit(t, 7);
    P.C = X > op;
    P.Z = t == 0;
    tick();
}

inline void CPU_6502::CPY(uint8_t op)
{
    uint8_t t = Y - op;
    P.N = bit(t, 7);
    P.C = Y > op;
    P.Z = t == 0;
    tick();
}

inline uint8_t CPU_6502::DEC(uint8_t op)
{
    uint8_t t = op - 1;
    P.N = bit(t, 7);
    P.Z = t == 0;
    tick();
    return t;
}

inline void CPU_6502::DEX()
{
    X--;
    P.Z = X == 0;
    P.N = bit(X, 7);
    tick();
}

inline void CPU_6502::DEY()
{
    Y--;
    P.Z = Y == 0;
    P.N = bit(Y, 7);
    tick();
}

inline void CPU_6502::EOR(uint8_t op)
{
    A ^= op;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

inline uint8_t CPU_6502::INC(uint8_t op)
{
    uint8_t t = op + 1;
    P.N = bit(t, 7);
    P.Z = t == 0;
    tick();
    return t;
}

inline void CPU_6502::INX()
{
    X++;
    P.Z = X == 0;
    P.N = bit(X, 7);
    tick();
}

inline void CPU_6502::INY()
{
    Y++;
    P.Z = Y == 0;
    P.N = bit(Y, 7);
    tick();
}

inline void CPU_6502::JMP(uint16_t address)
{
    PC = address;
    tick();
}

inline void CPU_6502::JSR()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t t = PC - 1;
    tick();
    push8((uint8_t)(t >> 8));
    tick();
    push8((uint8_t)t);
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    PC = (highByte << 8) | lowByte;
    tick();
}

inline void CPU_6502::LDA(uint8_t op)
{
    A = op;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

inline void CPU_6502::LDX(uint8_t op)
{
    X = op;
    P.N = bit(X, 7);
    P.Z = X == 0;
    tick();
}

inline void CPU_6502::LDY(uint8_t op)
{
    Y = op;
    P.N = bit(Y, 7);
    P.Z = Y == 0;
    tick();
}

inline uint8_t CPU_6502::LSR(uint8_t op)
{
    P.N = 0;
    P.C = bit(op, 0);
    uint8_t t = op >> 1;
    P.Z = t == 0;
    tick();
    return t;
}

inline void CPU_6502::NOP()
{
    tick();
}

inline void CPU_6502::ORA(uint8_t op)
{
    A |= op;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

inline void CPU_6502::PHA()
{
    tick();
    push8(A);
    tick();
}

inline void CPU_6502::PHP()
{
    tick();
    pushProcessorStatus();
    tick();
}

inline void CPU_6502::PLA()
{
    tick();
    tick();
    A = pull8();
    tick();
}

inline void CPU_6502::PLP()
{
    tick();
    tick();
    pullProcessorStatus();
    tick();
}

inline uint8_t CPU_6502::ROL(uint8_t op)
{
    uint8_t t = op << 1;
    t |= P.C;
    P.C = bit(op, 7);
    P.Z = t == 0;
    P.N = bit(t, 7);
    tick();
    return t;
}

inline uint8_t CPU_6502::ROR(uint8_t op)
{
    uint8_t t = op >> 1;
    t |= P.C == 1 ? 0x80 : 0x00;
    P.C = bit(op, 0);
    P.Z = t == 0;
    P.N = bit(t, 7);
    tick();
    return t;
}

inline void CPU_6502::RTI()
{
    tick();
    tick();
    pullProcessorStatus();
    tick();
    uint16_t lowByte = (uint16_t)pull8();
    tick();
    uint16_t highByte = (uint16_t)pull8();
    PC = (highByte << 8) | lowByte;
    tick();
}

inline void CPU_6502::RTS()
{
    tick();
    tick();
    uint16_t lowByte = (uint16_t)pull8();
    tick();
    uint16_t highByte = (uint16_t)pull8();
    PC = (highByte << 8) | lowByte;
    tick();
    PC++;
    tick();
}

inline void CPU_6502::SBC(uint8_t op)
{
    int16_t t = A - op - (PC == 1 ? 0 : 1);
    P.V = t > 127 || t < -128;
    P.C = t >= 0;
    P.N = bit((uint8_t)t, 7);
    P.Z = t == 0;
    A = (uint8_t)t;
    tick();
}

inline void CPU_6502::SEC()
{
    P.C = 1;
    tick();
}

inline void CPU_6502::SED()
{
    P.D = 1;
    tick();
}

inline void CPU_6502::SEI()
{
    P.I = 1;
    tick();
}

inline uint8_t CPU_6502::STA()
{
    return A;
}

inline uint8_t CPU_6502::STX()
{
    return X;
}

inline uint8_t CPU_6502::STY()
{
    return Y;
}

inline void CPU_6502::TAX()
{
    X = A;
    P.N = bit(X, 7);
    P.Z = X == 0;
    tick();
}

inline void CPU_6502::TAY()
{
    Y = A;
    P.N = bit(Y, 7);
    P.Z = Y == 0;
    tick();
}

inline void CPU_6502::TSX()
{
    X = SP;
    P.N = bit(X, 7);
    P.Z = X == 0;
    tick();
}

inline void CPU_6502::TXA()
{
    A = X;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

inline void CPU_6502::TXS()
{
    SP = X;
    P.N = bit(SP, 7);
    P.Z = SP == 0;
    tick();
}

inline void CPU_6502::TYA()
{
    A = Y;
    P.N = bit(A, 7);
    P.Z = A == 0;
    tick();
}

void CPU_6502::op0x00()
{
    BRK();
}

void CPU_6502::op0x01()
{
    uint8_t op = read8(INX_Address_R());
    ORA(op);
}

void CPU_6502::op0x02()
{
    NOOP();
}

void CPU_6502::op0x03()
{
    NOOP();
}

void CPU_6502::op0x04()
{
    NOOP();
}

inline uint8_t bit(uint8_t value, int index)
{
    return (value >> index) & 0x01;
}

inline uint16_t bit(uint16_t value, int index)
{
    return (value >> index) & 0x01;
}

void CPU_6502::op0x68()
{
    PLA();
}

void CPU_6502::op0x69()
{
    uint8_t op = read8(PC);
    PC++;
    ADC(op);
}

void CPU_6502::op0x6A()
{
    A = ROR(A);
}

void CPU_6502::op0x6B()
{
    NOOP();
}

void CPU_6502::op0x65()
{
    uint8_t op = read8(ZPA_Address_R());
    ADC(op);
}

void CPU_6502::op0x66()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = ROR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x67()
{
    NOOP();
}

void CPU_6502::op0x75()
{
    uint8_t op = read8(ZPX_Address_R());
    ADC(op);
}

void CPU_6502::op0x76()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = ROR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x77()
{
    NOOP();
}

void CPU_6502::op0x6C()
{
    uint16_t lowBytePointer = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highBytePointer = (uint16_t)read8(PC);
    PC++;
    uint16_t pointer = (highBytePointer << 8) | lowBytePointer;
    tick();
    uint16_t lowByte = (uint16_t)read8(pointer);
    tick();
    uint16_t highByte = (uint16_t)read8((pointer + 1) & 0x00FF);
    uint16_t address = (highByte << 8) | lowByte;
    JMP(address);
}

void CPU_6502::op0x6D()
{
    uint8_t op = read8(ABS_Address_R());
    ADC(op);
}

void CPU_6502::op0x6E()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = ROR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x6F()
{
    NOOP();
}

void CPU_6502::op0x7D()
{
    uint8_t op = read8(ABX_Address_R());
    ADC(op);
}

void CPU_6502::op0x7E()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = ROR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x7F()
{
    NOOP();
}

void CPU_6502::op0x80()
{
    NOOP();
}

void CPU_6502::op0x81()
{
    uint8_t r = STA();
    uint16_t address = INX_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x82()
{
    NOOP();
}

void CPU_6502::op0x83()
{
    NOOP();
}

void CPU_6502::op0x78()
{
    SEI();
}

void CPU_6502::op0x79()
{
    uint8_t op = read8(ABY_Address_R());
    ADC(op);
}

void CPU_6502::op0x7A()
{
    NOOP();
}

void CPU_6502::op0x7B()
{
    NOOP();
}

void CPU_6502::op0x7C()
{
    NOOP();
}

void CPU_6502::op0x60()
{
    RTS();
}

void CPU_6502::op0x61()
{
    uint8_t op = read8(INX_Address_R());
    ADC(op);
}

void CPU_6502::op0x62()
{
    NOOP();
}

void CPU_6502::op0x63()
{
    NOOP();
}

void CPU_6502::op0x64()
{
    NOOP();
}

void CPU_6502::op0x70()
{
    BVS();
}

void CPU_6502::op0x71()
{
    uint8_t op = read8(INY_Address_R());
    ADC(op);
}

void CPU_6502::op0x72()
{
    NOOP();
}

void CPU_6502::op0x73()
{
    NOOP();
}

void CPU_6502::op0x74()
{
    NOOP();
}

void CPU_6502::op0x28()
{
    PLP();
}

void CPU_6502::op0x29()
{
    uint8_t op = read8(PC);
    PC++;
    AND(op);
}

void CPU_6502::op0x2A()
{
    A = ROL(A);
}

void CPU_6502::op0x2B()
{
    NOOP();
}

void CPU_6502::op0x24()
{
    uint8_t op = read8(ZPA_Address_R());
    BIT(op);
}

void CPU_6502::op0x25()
{
    uint8_t op = ZPA_Address_R();
    AND(op);
}

void CPU_6502::op0x26()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = ROL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x27()
{
    NOOP();
}

void CPU_6502::op0x35()
{
    uint8_t op = ZPX_Address_R();
    AND(op);
}

void CPU_6502::op0x36()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = ROL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x37()
{
    NOOP();
}

void CPU_6502::op0x2C()
{
    uint8_t op = read8(ABS_Address_R());
    BIT(op);
}

void CPU_6502::op0x2D()
{
    uint8_t op = ABS_Address_R();
    AND(op);
}

void CPU_6502::op0x2E()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = ROL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x2F()
{
    NOOP();
}

void CPU_6502::op0x3D()
{
    uint8_t op = ABX_Address_R();
    AND(op);
}

void CPU_6502::op0x3E()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = ROL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x3F()
{
    NOOP();
}

void CPU_6502::op0x38()
{
    SEC();
}

void CPU_6502::op0x39()
{
    uint8_t op = ABY_Address_R();
    AND(op);
}

void CPU_6502::op0x3A()
{
    NOOP();
}

void CPU_6502::op0x3B()
{
    NOOP();
}

void CPU_6502::op0x3C()
{
    NOOP();
}

void CPU_6502::op0x1D()
{
    uint8_t op = read8(ABX_Address_R());
    ORA(op);
}

void CPU_6502::op0x1E()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = ASL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x1F()
{
    NOOP();
}

void CPU_6502::op0x20()
{
    JSR();
}

void CPU_6502::op0x21()
{
    uint8_t op = INX_Address_R();
    AND(op);
}

void CPU_6502::op0x22()
{
    NOOP();
}

void CPU_6502::op0x23()
{
    NOOP();
}

void CPU_6502::op0x30()
{
    BMI();
}

void CPU_6502::op0x31()
{
    uint8_t op = INY_Address_R();
    AND(op);
}

void CPU_6502::op0x32()
{
    NOOP();
}

void CPU_6502::op0x33()
{
    NOOP();
}

void CPU_6502::op0x34()
{
    NOOP();
}

void CPU_6502::op0x08()
{
    PHP();
}

void CPU_6502::op0x09()
{
    uint8_t op = read8(PC);
    PC++;
    ORA(op);
}

void CPU_6502::op0x0A()
{
    A = ASL(A);
}

void CPU_6502::op0x0B()
{
    NOOP();
}

void CPU_6502::op0x0C()
{
    NOOP();
}

void CPU_6502::op0x05()
{
    uint8_t op = read8(ZPA_Address_R());
    ORA(op);
}

void CPU_6502::op0x06()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = ASL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x07()
{
    NOOP();
}

void CPU_6502::op0x15()
{
    uint8_t op = read8(ZPX_Address_R());
    ORA(op);
}

void CPU_6502::op0x16()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = ASL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x17()
{
    NOOP();
}

void CPU_6502::op0x18()
{
    CLC();
}

void CPU_6502::op0x19()
{
    uint8_t op = read8(ABY_Address_R());
    ORA(op);
}

void CPU_6502::op0x1A()
{
    NOOP();
}

void CPU_6502::op0x1B()
{
    NOOP();
}

void CPU_6502::op0x1C()
{
    NOOP();
}

void CPU_6502::op0x0D()
{
    uint8_t op = read8(ABS_Address_R());
    ORA(op);
}

void CPU_6502::op0x0E()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = ASL(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x0F()
{
    NOOP();
}

void CPU_6502::op0x10()
{
    BPL();
}

void CPU_6502::op0x11()
{
    uint8_t op = read8(INY_Address_R());
    ORA(op);
}

void CPU_6502::op0x12()
{
    NOOP();
}

void CPU_6502::op0x13()
{
    NOOP();
}

void CPU_6502::op0x14()
{
    NOOP();
}

void CPU_6502::op0x8C()
{
    uint8_t r = STY();
    uint16_t address = ABS_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x8D()
{
    uint8_t r = STA();
    uint16_t address = ABS_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x8E()
{
    uint8_t r = STX();
    uint16_t address = ABS_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x8F()
{
    NOOP();
}

void CPU_6502::op0x90()
{
    BCC();
}

void CPU_6502::op0x91()
{
    uint8_t r = STA();
    uint16_t address = INY_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x92()
{
    NOOP();
}

void CPU_6502::op0x93()
{
    NOOP();
}

void CPU_6502::op0xAC()
{
    uint8_t op = read8(ABS_Address_R());
    LDY(op);
}

void CPU_6502::op0xAD()
{
    uint8_t op = read8(ABS_Address_R());
    LDA(op);
}

void CPU_6502::op0xAE()
{
    uint8_t op = read8(ABS_Address_R());
    LDX(op);
}

void CPU_6502::op0xAF()
{
    NOOP();
}

void CPU_6502::op0xB0()
{
    BCS();
}

void CPU_6502::op0xB1()
{
    uint8_t op = read8(INY_Address_R());
    LDA(op);
}

void CPU_6502::op0xB2()
{
    NOOP();
}

void CPU_6502::op0xB3()
{
    NOOP();
}

void CPU_6502::op0xF0()
{
    BEQ();
}

void CPU_6502::op0xF1()
{
    uint8_t op = read8(INY_Address_R());
    SBC(op);
}

void CPU_6502::op0xF2()
{
    NOOP();
}

void CPU_6502::op0xF3()
{
    NOOP();
}

void CPU_6502::op0xF4()
{
    NOOP();
}

void CPU_6502::op0xCC()
{
    uint8_t op = read8(ABS_Address_R());
    CPY(op);
}

void CPU_6502::op0xCD()
{
    uint8_t op = read8(ABS_Address_R());
    CMP(op);
}

void CPU_6502::op0xCE()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = DEC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xCF()
{
    NOOP();
}

void CPU_6502::op0xD0()
{
    BNE();
}

void CPU_6502::op0xD1()
{
    uint8_t op = read8(INY_Address_R());
    CMP(op);
}

void CPU_6502::op0xD2()
{
    NOOP();
}

void CPU_6502::op0xD3()
{
    NOOP();
}

void CPU_6502::op0xD4()
{
    NOOP();
}

void CPU_6502::op0x4C()
{
    uint16_t lowByte = (uint16_t)read8(PC);
    PC++;
    tick();
    uint16_t highByte = (uint16_t)read8(PC);
    uint16_t address = (highByte << 8) | lowByte;
    JMP(address);
}

void CPU_6502::op0x4D()
{
    uint8_t op = read8(ABS_Address_R());
    EOR(op);
}

void CPU_6502::op0x4E()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = LSR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x4F()
{
    NOOP();
}

void CPU_6502::op0x50()
{
    BVC();
}

void CPU_6502::op0x51()
{
    uint8_t op = read8(INY_Address_R());
    EOR(op);
}

void CPU_6502::op0x52()
{
    NOOP();
}

void CPU_6502::op0x53()
{
    NOOP();
}

void CPU_6502::op0x54()
{
    NOOP();
}

void CPU_6502::op0xD5()
{
    uint8_t op = read8(ZPX_Address_R());
    CMP(op);
}

void CPU_6502::op0xD6()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = DEC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xD7()
{
    NOOP();
}

void CPU_6502::op0xD8()
{
    CLD();
}

void CPU_6502::op0xD9()
{
    uint8_t op = read8(ABY_Address_R());
    CMP(op);
}

void CPU_6502::op0xDA()
{
    NOOP();
}

void CPU_6502::op0xDB()
{
    NOOP();
}

void CPU_6502::op0xDC()
{
    NOOP();
}

void CPU_6502::op0x55()
{
    uint8_t op = read8(ZPX_Address_R());
    EOR(op);
}

void CPU_6502::op0x56()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = LSR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x57()
{
    NOOP();
}

void CPU_6502::op0x58()
{
    CLI();
}

void CPU_6502::op0x59()
{
    uint8_t op = read8(ABY_Address_R());
    EOR(op);
}

void CPU_6502::op0x5A()
{
    NOOP();
}

void CPU_6502::op0x5B()
{
    NOOP();
}

void CPU_6502::op0x5C()
{
    NOOP();
}

void CPU_6502::op0xB4()
{
    uint8_t op = read8(ZPX_Address_R());
    LDY(op);
}

void CPU_6502::op0xB5()
{
    uint8_t op = read8(ZPX_Address_R());
    LDA(op);
}

void CPU_6502::op0xB6()
{
    uint8_t op = read8(ZPY_Address_R());
    LDX(op);
}

void CPU_6502::op0xB7()
{
    NOOP();
}

void CPU_6502::op0xB8()
{
    CLV();
}

void CPU_6502::op0xB9()
{
    uint8_t op = read8(ABY_Address_R());
    LDA(op);
}

void CPU_6502::op0xBA()
{
    TSX();
}

void CPU_6502::op0xBB()
{
    NOOP();
}

void CPU_6502::op0xC8()
{
    INY();
}

void CPU_6502::op0xC9()
{
    uint8_t op = read8(PC);
    PC++;
    CMP(op);
}

void CPU_6502::op0xCA()
{
    DEX();
}

void CPU_6502::op0xCB()
{
    NOOP();
}

void CPU_6502::op0xC4()
{
    uint8_t op = read8(ZPA_Address_R());
    CPY(op);
}

void CPU_6502::op0xC5()
{
    uint8_t op = read8(ZPA_Address_R());
    CMP(op);
}

void CPU_6502::op0xC6()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = DEC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xC7()
{
    NOOP();
}

void CPU_6502::op0xDD()
{
    uint8_t op = read8(ABX_Address_R());
    CMP(op);
}

void CPU_6502::op0xDE()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = DEC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xDF()
{
    NOOP();
}

void CPU_6502::op0xE0()
{
    uint8_t op = read8(PC);
    PC++;
    CPX(op);
}

void CPU_6502::op0xE1()
{
    uint8_t op = read8(INX_Address_R());
    SBC(op);
}

void CPU_6502::op0xE2()
{
    NOOP();
}

void CPU_6502::op0xE3()
{
    NOOP();
}

void CPU_6502::op0xBC()
{
    uint8_t op = read8(ABX_Address_R());
    LDY(op);
}

void CPU_6502::op0xBD()
{
    uint8_t op = read8(ABX_Address_R());
    LDA(op);
}

void CPU_6502::op0xBE()
{
    uint8_t op = read8(ABY_Address_R());
    LDX(op);
}

void CPU_6502::op0xBF()
{
    NOOP();
}

void CPU_6502::op0xC0()
{
    uint8_t op = read8(PC);
    PC++;
    CPY(op);
}

void CPU_6502::op0xC1()
{
    uint8_t op = read8(INX_Address_R());
    CMP(op);
}

void CPU_6502::op0xC2()
{
    NOOP();
}

void CPU_6502::op0xC3()
{
    NOOP();
}

void CPU_6502::op0xE4()
{
    uint8_t op = read8(ZPA_Address_R());
    CPX(op);
}

void CPU_6502::op0xE5()
{
    uint8_t op = read8(ZPA_Address_R());
    SBC(op);
}

void CPU_6502::op0xE6()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = INC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xE7()
{
    NOOP();
}

void CPU_6502::op0xE8()
{
    INX();
}

void CPU_6502::op0xE9()
{
    uint8_t op = read8(PC);
    PC++;
    SBC(op);
}

void CPU_6502::op0xEA()
{
    NOP();
}

void CPU_6502::op0xEB()
{
    NOOP();
}

void CPU_6502::op0xEC()
{
    uint8_t op = read8(ABS_Address_R());
    CPX(op);
}

void CPU_6502::op0xED()
{
    uint8_t op = read8(ABS_Address_R());
    SBC(op);
}

void CPU_6502::op0xEE()
{
    uint16_t address = ABS_Address_RW();
    uint8_t r = INC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xEF()
{
    NOOP();
}

void CPU_6502::op0x84()
{
    uint8_t r = STY();
    uint16_t address = ZPA_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x85()
{
    uint8_t r = STA();
    uint16_t address = ZPA_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x86()
{
    uint8_t r = STX();
    uint16_t address = ZPA_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x87()
{
    NOOP();
}

void CPU_6502::op0x88()
{
    DEY();
}

void CPU_6502::op0x89()
{
    NOOP();
}

void CPU_6502::op0x8A()
{
    TXA();
}

void CPU_6502::op0x8B()
{
    NOOP();
}

void CPU_6502::op0x48()
{
    PHA();
}

void CPU_6502::op0x49()
{
    uint8_t op = read8(PC);
    PC++;
    EOR(op);
}

void CPU_6502::op0x4A()
{
    A = LSR(A);
}

void CPU_6502::op0x4B()
{
    NOOP();
}

void CPU_6502::op0x45()
{
    uint8_t op = read8(ZPA_Address_R());
    EOR(op);
}

void CPU_6502::op0x46()
{
    uint16_t address = ZPA_Address_RW();
    uint8_t r = LSR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x47()
{
    NOOP();
}

void CPU_6502::op0x5D()
{
    uint8_t op = read8(ABX_Address_R());
    EOR(op);
}

void CPU_6502::op0x5E()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = LSR(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0x5F()
{
    NOOP();
}

void CPU_6502::op0x40()
{
    RTI();
}

void CPU_6502::op0x41()
{
    uint8_t op = read8(INX_Address_R());
    EOR(op);
}

void CPU_6502::op0x42()
{
    NOOP();
}

void CPU_6502::op0x43()
{
    NOOP();
}

void CPU_6502::op0x44()
{
    NOOP();
}

void CPU_6502::op0xF5()
{
    uint8_t op = read8(ZPX_Address_R());
    SBC(op);
}

void CPU_6502::op0xF6()
{
    uint16_t address = ZPX_Address_RW();
    uint8_t r = INC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xF7()
{
    NOOP();
}

void CPU_6502::op0xF8()
{
    SED();
}

void CPU_6502::op0xF9()
{
    uint8_t op = read8(ABY_Address_R());
    SBC(op);
}

void CPU_6502::op0xFA()
{
    NOOP();
}

void CPU_6502::op0xFB()
{
    NOOP();
}

void CPU_6502::op0xFC()
{
    NOOP();
}

void CPU_6502::op0xFD()
{
    uint8_t op = read8(ABX_Address_R());
    SBC(op);
}

void CPU_6502::op0xFE()
{
    uint16_t address = ABX_Address_RW();
    uint8_t r = INC(read8(address));
    tick();
    write8(address, r);
    tick();
}

void CPU_6502::op0xFF()
{
    NOOP();
}

inline void CPU_6502::NOOP()
{
    std::cout << "NOOP called at 0x";
    std::cout << std::uppercase << std::hex << PC << std::dec << std::nouppercase;
    std::cout << "!" << std::endl;
}

void CPU_6502::op0xA8()
{
    TAY();
}

void CPU_6502::op0xA9()
{
    uint8_t op = read8(PC);
    PC++;
    LDA(op);
}

void CPU_6502::op0xAA()
{
    TAX();
}

void CPU_6502::op0xAB()
{
    NOOP();
}

void CPU_6502::op0xA4()
{
    uint8_t op = read8(ZPA_Address_R());
    LDY(op);
}

void CPU_6502::op0xA5()
{
    uint8_t op = read8(ZPA_Address_R());
    LDA(op);
}

void CPU_6502::op0xA6()
{
    uint8_t op = read8(ZPA_Address_R());
    LDX(op);
}

void CPU_6502::op0xA7()
{
    NOOP();
}

void CPU_6502::op0x9D()
{
    uint8_t r = STA();
    uint16_t address = ABX_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x9E()
{
    NOOP();
}

void CPU_6502::op0x9F()
{
    NOOP();
}

void CPU_6502::op0xA0()
{
    uint8_t op = read8(PC);
    PC++;
    LDY(op);
}

void CPU_6502::op0xA1()
{
    uint8_t op = read8(INX_Address_R());
    LDA(op);
}

void CPU_6502::op0xA2()
{
    uint8_t op = read8(PC);
    PC++;
    LDX(op);
}

void CPU_6502::op0xA3()
{
    NOOP();
}

void CPU_6502::op0x94()
{
    uint8_t r = STY();
    uint16_t address = ZPX_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x95()
{
    uint8_t r = STA();
    uint16_t address = ZPX_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x96()
{
    uint8_t r = STX();
    uint16_t address = ZPY_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x97()
{
    NOOP();
}

void CPU_6502::op0x98()
{
    TYA();
}

void CPU_6502::op0x99()
{
    uint8_t r = STA();
    uint16_t address = ABY_Address_W();
    write8(address, r);
    tick();
}

void CPU_6502::op0x9A()
{
    TXS();
}

void CPU_6502::op0x9B()
{
    NOOP();
}

void CPU_6502::op0x9C()
{
    NOOP();
}
