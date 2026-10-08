#include "cpu.hpp"

namespace w65c02s {

namespace {

constexpr std::uint16_t stack_page = 0x0100;

constexpr std::uint8_t lo(std::uint16_t value) { return static_cast<std::uint8_t>(value); }
constexpr std::uint8_t hi(std::uint16_t value) { return static_cast<std::uint8_t>(value >> 8); }

} // namespace

void Cpu::reset()
{
    r_.s = 0xFD;
    r_.p = static_cast<std::uint8_t>((r_.p | flag::U | flag::I) & ~flag::D);
    r_.pc = mem_.read_word(vector::reset);
    stopped_ = false;
    waiting_ = false;
}

void Cpu::step()
{
    if (stopped_ || waiting_) {
        return;
    }
    execute(fetch());
}

void Cpu::irq()
{
    if (stopped_) {
        return;
    }
    // WAI resumes on IRQ even when interrupts are masked; execution then continues after WAI.
    waiting_ = false;
    if (!flag_set(flag::I)) {
        interrupt(vector::irq, false);
    }
}

void Cpu::nmi()
{
    if (stopped_) {
        return;
    }
    waiting_ = false;
    interrupt(vector::nmi, false);
}

void Cpu::interrupt(std::uint16_t vector_address, bool software)
{
    push(r_.pc);
    push(static_cast<std::uint8_t>((software ? r_.p | flag::B : r_.p & ~flag::B) | flag::U));
    r_.p = static_cast<std::uint8_t>((r_.p | flag::I) & ~flag::D);
    r_.pc = mem_.read_word(vector_address);
}

std::uint8_t Cpu::fetch() { return mem_.read(r_.pc++); }

std::uint16_t Cpu::fetch_word()
{
    const std::uint8_t low = fetch();
    return static_cast<std::uint16_t>(low | fetch() << 8);
}

void Cpu::push(std::uint8_t value) {
    mem_.write(stack_page | r_.s--, value);
}

void Cpu::push(const std::uint16_t value) {
    push(hi(value));
    push(lo(value));
}

std::uint8_t Cpu::pull() {
    return mem_.read(stack_page | ++r_.s);
}

std::uint16_t Cpu::pull_word()
{
    const std::uint8_t low = pull();
    return static_cast<std::uint16_t>(low | pull() << 8);
}

std::uint16_t Cpu::imm() { return r_.pc++; }
std::uint16_t Cpu::zp() { return fetch(); }
std::uint16_t Cpu::zpx() { return static_cast<std::uint8_t>(fetch() + r_.x); }
std::uint16_t Cpu::zpy() { return static_cast<std::uint8_t>(fetch() + r_.y); }
std::uint16_t Cpu::abs() { return fetch_word(); }
std::uint16_t Cpu::absx() { return static_cast<std::uint16_t>(fetch_word() + r_.x); }
std::uint16_t Cpu::absy() { return static_cast<std::uint16_t>(fetch_word() + r_.y); }
std::uint16_t Cpu::zpi() { return read_zp_word(fetch()); }
std::uint16_t Cpu::zpxi() { return read_zp_word(static_cast<std::uint8_t>(fetch() + r_.x)); }
std::uint16_t Cpu::zpyi() { return static_cast<std::uint16_t>(read_zp_word(fetch()) + r_.y); }

// Pointers stored in the zero page wrap within it ($FF -> $00).
std::uint16_t Cpu::read_zp_word(std::uint8_t address) const
{
    return static_cast<std::uint16_t>(mem_.read(address) | mem_.read(static_cast<std::uint8_t>(address + 1)) << 8);
}

void Cpu::set_flag(std::uint8_t mask, bool on) { r_.p = static_cast<std::uint8_t>(on ? r_.p | mask : r_.p & ~mask); }

std::uint8_t Cpu::set_nz(std::uint8_t value)
{
    set_flag(flag::Z, value == 0);
    set_flag(flag::N, (value & 0x80) != 0);
    return value;
}

void Cpu::load(std::uint8_t& reg, std::uint16_t address) { reg = set_nz(mem_.read(address)); }

void Cpu::store(std::uint8_t value, std::uint16_t address) { mem_.write(address, value); }

void Cpu::adc(std::uint8_t value)
{
    const int carry = flag_set(flag::C) ? 1 : 0;

    if (!flag_set(flag::D)) {
        const int sum = r_.a + value + carry;
        const auto result = static_cast<std::uint8_t>(sum);
        set_flag(flag::C, sum > 0xFF);
        set_flag(flag::V, ((r_.a ^ result) & (value ^ result) & 0x80) != 0);
        r_.a = set_nz(result);
        return;
    }

    // BCD addition as done by the 65C02 (N and Z are valid, V follows the binary-like intermediate).
    int low = (r_.a & 0x0F) + (value & 0x0F) + carry;
    if (low >= 0x0A) {
        low = ((low + 0x06) & 0x0F) + 0x10;
    }
    const int signed_sum = static_cast<std::int8_t>(r_.a & 0xF0) + static_cast<std::int8_t>(value & 0xF0) + low;
    int sum = (r_.a & 0xF0) + (value & 0xF0) + low;
    if (sum >= 0xA0) {
        sum += 0x60;
    }
    set_flag(flag::V, signed_sum < -128 || signed_sum > 127);
    set_flag(flag::C, sum > 0xFF);
    r_.a = set_nz(static_cast<std::uint8_t>(sum));
}

void Cpu::sbc(std::uint8_t value)
{
    if (!flag_set(flag::D)) {
        adc(static_cast<std::uint8_t>(~value));
        return;
    }

    const int borrow = flag_set(flag::C) ? 0 : 1;
    const int difference = r_.a - value - borrow;
    const auto binary = static_cast<std::uint8_t>(difference);
    set_flag(flag::C, difference >= 0);
    set_flag(flag::V, ((r_.a ^ value) & (r_.a ^ binary) & 0x80) != 0);

    const int low = (r_.a & 0x0F) - (value & 0x0F) - borrow;
    int result = difference;
    if (result < 0) {
        result -= 0x60;
    }
    if (low < 0) {
        result -= 0x06;
    }
    r_.a = set_nz(static_cast<std::uint8_t>(result));
}

void Cpu::compare(std::uint8_t reg, std::uint8_t value)
{
    set_flag(flag::C, reg >= value);
    set_nz(static_cast<std::uint8_t>(reg - value));
}

void Cpu::bit(std::uint8_t value, bool immediate)
{
    set_flag(flag::Z, (r_.a & value) == 0);
    if (!immediate) {
        set_flag(flag::N, (value & 0x80) != 0);
        set_flag(flag::V, (value & 0x40) != 0);
    }
}

void Cpu::tsb(std::uint16_t address)
{
    const std::uint8_t value = mem_.read(address);
    set_flag(flag::Z, (r_.a & value) == 0);
    mem_.write(address, value | r_.a);
}

void Cpu::trb(std::uint16_t address)
{
    const std::uint8_t value = mem_.read(address);
    set_flag(flag::Z, (r_.a & value) == 0);
    mem_.write(address, static_cast<std::uint8_t>(value & ~r_.a));
}

void Cpu::branch(bool condition)
{
    const auto offset = static_cast<std::int8_t>(fetch());
    if (condition) {
        r_.pc = static_cast<std::uint16_t>(r_.pc + offset);
    }
}

void Cpu::branch_on_bit(unsigned bit, bool set)
{
    const std::uint8_t value = mem_.read(zp());
    branch(((value >> bit) & 1U) == (set ? 1U : 0U));
}

void Cpu::set_memory_bit(unsigned bit, bool set)
{
    const std::uint16_t address = zp();
    const auto mask = static_cast<std::uint8_t>(1U << bit);
    const std::uint8_t value = mem_.read(address);
    mem_.write(address, static_cast<std::uint8_t>(set ? value | mask : value & ~mask));
}

std::uint8_t Cpu::asl(std::uint8_t value)
{
    set_flag(flag::C, (value & 0x80) != 0);
    return set_nz(static_cast<std::uint8_t>(value << 1));
}

std::uint8_t Cpu::lsr(std::uint8_t value)
{
    set_flag(flag::C, (value & 0x01) != 0);
    return set_nz(static_cast<std::uint8_t>(value >> 1));
}

std::uint8_t Cpu::rol(std::uint8_t value)
{
    const unsigned carry_in = flag_set(flag::C) ? 0x01 : 0x00;
    set_flag(flag::C, (value & 0x80) != 0);
    return set_nz(static_cast<std::uint8_t>((value << 1) | carry_in));
}

std::uint8_t Cpu::ror(std::uint8_t value)
{
    const unsigned carry_in = flag_set(flag::C) ? 0x80 : 0x00;
    set_flag(flag::C, (value & 0x01) != 0);
    return set_nz(static_cast<std::uint8_t>((value >> 1) | carry_in));
}

template <typename Op>
void Cpu::modify(std::uint16_t address, Op op)
{
    mem_.write(address, op(mem_.read(address)));
}

void Cpu::execute(std::uint8_t opcode)
{
    const auto inc = [this](std::uint8_t v) { return set_nz(static_cast<std::uint8_t>(v + 1)); };
    const auto dec = [this](std::uint8_t v) { return set_nz(static_cast<std::uint8_t>(v - 1)); };
    const auto asl_op = [this](std::uint8_t v) { return asl(v); };
    const auto lsr_op = [this](std::uint8_t v) { return lsr(v); };
    const auto rol_op = [this](std::uint8_t v) { return rol(v); };
    const auto ror_op = [this](std::uint8_t v) { return ror(v); };
    const auto read = [this](std::uint16_t address) { return mem_.read(address); };

    // clang-format off
    switch (opcode) {
    // Loads
    case 0xA9: load(r_.a, imm()); break;
    case 0xA5: load(r_.a, zp()); break;
    case 0xB5: load(r_.a, zpx()); break;
    case 0xAD: load(r_.a, abs()); break;
    case 0xBD: load(r_.a, absx()); break;
    case 0xB9: load(r_.a, absy()); break;
    case 0xA1: load(r_.a, zpxi()); break;
    case 0xB1: load(r_.a, zpyi()); break;
    case 0xB2: load(r_.a, zpi()); break;
    case 0xA2: load(r_.x, imm()); break;
    case 0xA6: load(r_.x, zp()); break;
    case 0xB6: load(r_.x, zpy()); break;
    case 0xAE: load(r_.x, abs()); break;
    case 0xBE: load(r_.x, absy()); break;
    case 0xA0: load(r_.y, imm()); break;
    case 0xA4: load(r_.y, zp()); break;
    case 0xB4: load(r_.y, zpx()); break;
    case 0xAC: load(r_.y, abs()); break;
    case 0xBC: load(r_.y, absx()); break;

    // Stores
    case 0x85: store(r_.a, zp()); break;
    case 0x95: store(r_.a, zpx()); break;
    case 0x8D: store(r_.a, abs()); break;
    case 0x9D: store(r_.a, absx()); break;
    case 0x99: store(r_.a, absy()); break;
    case 0x81: store(r_.a, zpxi()); break;
    case 0x91: store(r_.a, zpyi()); break;
    case 0x92: store(r_.a, zpi()); break;
    case 0x86: store(r_.x, zp()); break;
    case 0x96: store(r_.x, zpy()); break;
    case 0x8E: store(r_.x, abs()); break;
    case 0x84: store(r_.y, zp()); break;
    case 0x94: store(r_.y, zpx()); break;
    case 0x8C: store(r_.y, abs()); break;
    case 0x64: store(0, zp()); break;
    case 0x74: store(0, zpx()); break;
    case 0x9C: store(0, abs()); break;
    case 0x9E: store(0, absx()); break;

    // Register transfers
    case 0xAA: r_.x = set_nz(r_.a); break; // TAX
    case 0xA8: r_.y = set_nz(r_.a); break; // TAY
    case 0x8A: r_.a = set_nz(r_.x); break; // TXA
    case 0x98: r_.a = set_nz(r_.y); break; // TYA
    case 0xBA: r_.x = set_nz(r_.s); break; // TSX
    case 0x9A: r_.s = r_.x; break;         // TXS

    // Stack
    case 0x48: push(r_.a); break;                                                      // PHA
    case 0xDA: push(r_.x); break;                                                      // PHX
    case 0x5A: push(r_.y); break;                                                      // PHY
    case 0x08: push(static_cast<uint8_t>(r_.p | flag::B | flag::U)); break;            // PHP
    case 0x68: r_.a = set_nz(pull()); break;                                      // PLA
    case 0xFA: r_.x = set_nz(pull()); break;                                      // PLX
    case 0x7A: r_.y = set_nz(pull()); break;                                      // PLY
    case 0x28: r_.p = static_cast<std::uint8_t>((pull() & ~flag::B) | flag::U); break; // PLP

    // Logic
    case 0x29: r_.a = set_nz(r_.a & read(imm())); break;
    case 0x25: r_.a = set_nz(r_.a & read(zp())); break;
    case 0x35: r_.a = set_nz(r_.a & read(zpx())); break;
    case 0x2D: r_.a = set_nz(r_.a & read(abs())); break;
    case 0x3D: r_.a = set_nz(r_.a & read(absx())); break;
    case 0x39: r_.a = set_nz(r_.a & read(absy())); break;
    case 0x21: r_.a = set_nz(r_.a & read(zpxi())); break;
    case 0x31: r_.a = set_nz(r_.a & read(zpyi())); break;
    case 0x32: r_.a = set_nz(r_.a & read(zpi())); break;
    case 0x09: r_.a = set_nz(r_.a | read(imm())); break;
    case 0x05: r_.a = set_nz(r_.a | read(zp())); break;
    case 0x15: r_.a = set_nz(r_.a | read(zpx())); break;
    case 0x0D: r_.a = set_nz(r_.a | read(abs())); break;
    case 0x1D: r_.a = set_nz(r_.a | read(absx())); break;
    case 0x19: r_.a = set_nz(r_.a | read(absy())); break;
    case 0x01: r_.a = set_nz(r_.a | read(zpxi())); break;
    case 0x11: r_.a = set_nz(r_.a | read(zpyi())); break;
    case 0x12: r_.a = set_nz(r_.a | read(zpi())); break;
    case 0x49: r_.a = set_nz(r_.a ^ read(imm())); break;
    case 0x45: r_.a = set_nz(r_.a ^ read(zp())); break;
    case 0x55: r_.a = set_nz(r_.a ^ read(zpx())); break;
    case 0x4D: r_.a = set_nz(r_.a ^ read(abs())); break;
    case 0x5D: r_.a = set_nz(r_.a ^ read(absx())); break;
    case 0x59: r_.a = set_nz(r_.a ^ read(absy())); break;
    case 0x41: r_.a = set_nz(r_.a ^ read(zpxi())); break;
    case 0x51: r_.a = set_nz(r_.a ^ read(zpyi())); break;
    case 0x52: r_.a = set_nz(r_.a ^ read(zpi())); break;
    case 0x89: bit(read(imm()), true); break;
    case 0x24: bit(read(zp()), false); break;
    case 0x34: bit(read(zpx()), false); break;
    case 0x2C: bit(read(abs()), false); break;
    case 0x3C: bit(read(absx()), false); break;
    case 0x04: tsb(zp()); break;
    case 0x0C: tsb(abs()); break;
    case 0x14: trb(zp()); break;
    case 0x1C: trb(abs()); break;

    // Arithmetic
    case 0x69: adc(read(imm())); break;
    case 0x65: adc(read(zp())); break;
    case 0x75: adc(read(zpx())); break;
    case 0x6D: adc(read(abs())); break;
    case 0x7D: adc(read(absx())); break;
    case 0x79: adc(read(absy())); break;
    case 0x61: adc(read(zpxi())); break;
    case 0x71: adc(read(zpyi())); break;
    case 0x72: adc(read(zpi())); break;
    case 0xE9: sbc(read(imm())); break;
    case 0xE5: sbc(read(zp())); break;
    case 0xF5: sbc(read(zpx())); break;
    case 0xED: sbc(read(abs())); break;
    case 0xFD: sbc(read(absx())); break;
    case 0xF9: sbc(read(absy())); break;
    case 0xE1: sbc(read(zpxi())); break;
    case 0xF1: sbc(read(zpyi())); break;
    case 0xF2: sbc(read(zpi())); break;
    case 0xC9: compare(r_.a, read(imm())); break;
    case 0xC5: compare(r_.a, read(zp())); break;
    case 0xD5: compare(r_.a, read(zpx())); break;
    case 0xCD: compare(r_.a, read(abs())); break;
    case 0xDD: compare(r_.a, read(absx())); break;
    case 0xD9: compare(r_.a, read(absy())); break;
    case 0xC1: compare(r_.a, read(zpxi())); break;
    case 0xD1: compare(r_.a, read(zpyi())); break;
    case 0xD2: compare(r_.a, read(zpi())); break;
    case 0xE0: compare(r_.x, read(imm())); break;
    case 0xE4: compare(r_.x, read(zp())); break;
    case 0xEC: compare(r_.x, read(abs())); break;
    case 0xC0: compare(r_.y, read(imm())); break;
    case 0xC4: compare(r_.y, read(zp())); break;
    case 0xCC: compare(r_.y, read(abs())); break;

    // Increments and decrements
    case 0x1A: r_.a = inc(r_.a); break;
    case 0xE8: r_.x = inc(r_.x); break;
    case 0xC8: r_.y = inc(r_.y); break;
    case 0xE6: modify(zp(), inc); break;
    case 0xF6: modify(zpx(), inc); break;
    case 0xEE: modify(abs(), inc); break;
    case 0xFE: modify(absx(), inc); break;
    case 0x3A: r_.a = dec(r_.a); break;
    case 0xCA: r_.x = dec(r_.x); break;
    case 0x88: r_.y = dec(r_.y); break;
    case 0xC6: modify(zp(), dec); break;
    case 0xD6: modify(zpx(), dec); break;
    case 0xCE: modify(abs(), dec); break;
    case 0xDE: modify(absx(), dec); break;

    // Shifts and rotates
    case 0x0A: r_.a = asl(r_.a); break;
    case 0x06: modify(zp(), asl_op); break;
    case 0x16: modify(zpx(), asl_op); break;
    case 0x0E: modify(abs(), asl_op); break;
    case 0x1E: modify(absx(), asl_op); break;
    case 0x4A: r_.a = lsr(r_.a); break;
    case 0x46: modify(zp(), lsr_op); break;
    case 0x56: modify(zpx(), lsr_op); break;
    case 0x4E: modify(abs(), lsr_op); break;
    case 0x5E: modify(absx(), lsr_op); break;
    case 0x2A: r_.a = rol(r_.a); break;
    case 0x26: modify(zp(), rol_op); break;
    case 0x36: modify(zpx(), rol_op); break;
    case 0x2E: modify(abs(), rol_op); break;
    case 0x3E: modify(absx(), rol_op); break;
    case 0x6A: r_.a = ror(r_.a); break;
    case 0x66: modify(zp(), ror_op); break;
    case 0x76: modify(zpx(), ror_op); break;
    case 0x6E: modify(abs(), ror_op); break;
    case 0x7E: modify(absx(), ror_op); break;

    // Jumps and subroutines
    case 0x4C: r_.pc = abs(); break;
    case 0x6C: r_.pc = mem_.read_word(abs()); break;
    case 0x7C: r_.pc = mem_.read_word(absx()); break;
    case 0x20: {
        const std::uint16_t target = fetch_word();
        push(static_cast<std::uint16_t>(r_.pc - 1));
        r_.pc = target;
        break;
    }
    case 0x60: r_.pc = static_cast<std::uint16_t>(pull_word() + 1); break;

    // Branches
    case 0x80: branch(true); break;
    case 0x10: branch(!flag_set(flag::N)); break;
    case 0x30: branch(flag_set(flag::N)); break;
    case 0x50: branch(!flag_set(flag::V)); break;
    case 0x70: branch(flag_set(flag::V)); break;
    case 0x90: branch(!flag_set(flag::C)); break;
    case 0xB0: branch(flag_set(flag::C)); break;
    case 0xD0: branch(!flag_set(flag::Z)); break;
    case 0xF0: branch(flag_set(flag::Z)); break;

    // Flags
    case 0x18: set_flag(flag::C, false); break;
    case 0x38: set_flag(flag::C, true); break;
    case 0x58: set_flag(flag::I, false); break;
    case 0x78: set_flag(flag::I, true); break;
    case 0xD8: set_flag(flag::D, false); break;
    case 0xF8: set_flag(flag::D, true); break;
    case 0xB8: set_flag(flag::V, false); break;

    // Interrupts and system
    case 0x00:
        ++r_.pc; // BRK skips its signature byte
        interrupt(vector::irq, true);
        break;
    case 0x40:
        r_.p = static_cast<std::uint8_t>((pull() & ~flag::B) | flag::U);
        r_.pc = pull_word();
        break;
    case 0xCB: waiting_ = true; break; // WAI
    case 0xDB: stopped_ = true; break; // STP
    case 0xEA: break;                  // NOP

    // Rockwell bit instructions: RMBn/SMBn (x7) and BBRn/BBSn (xF), bit number in the high nibble
    case 0x07: case 0x17: case 0x27: case 0x37: case 0x47: case 0x57: case 0x67: case 0x77:
        set_memory_bit(opcode >> 4, false);
        break;
    case 0x87: case 0x97: case 0xA7: case 0xB7: case 0xC7: case 0xD7: case 0xE7: case 0xF7:
        set_memory_bit((opcode >> 4) & 0x07, true);
        break;
    case 0x0F: case 0x1F: case 0x2F: case 0x3F: case 0x4F: case 0x5F: case 0x6F: case 0x7F:
        branch_on_bit(opcode >> 4, false);
        break;
    case 0x8F: case 0x9F: case 0xAF: case 0xBF: case 0xCF: case 0xDF: case 0xEF: case 0xFF:
        branch_on_bit((opcode >> 4) & 0x07, true);
        break;

    // Reserved opcodes are NOPs on the W65C02S, with defined lengths
    case 0x02: case 0x22: case 0x42: case 0x62: case 0x82: case 0xC2: case 0xE2:
    case 0x44: case 0x54: case 0xD4: case 0xF4:
        r_.pc = static_cast<std::uint16_t>(r_.pc + 1);
        break;
    case 0x5C: case 0xDC: case 0xFC:
        r_.pc = static_cast<std::uint16_t>(r_.pc + 2);
        break;
    default: // remaining x3 and xB opcodes: single-byte NOPs
        break;
    }
    // clang-format on
}

} // namespace w65c02s