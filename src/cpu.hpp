#pragma once

#include <cstdint>
#include "memory.hpp"

namespace w65c02s {

namespace flag {
inline constexpr std::uint8_t C = 0x01; // carry
inline constexpr std::uint8_t Z = 0x02; // zero
inline constexpr std::uint8_t I = 0x04; // IRQ disable
inline constexpr std::uint8_t D = 0x08; // decimal mode
inline constexpr std::uint8_t B = 0x10; // break
inline constexpr std::uint8_t U = 0x20; // unused
inline constexpr std::uint8_t V = 0x40; // overflow
inline constexpr std::uint8_t N = 0x80; // negative
} // namespace flag

namespace vector {
inline constexpr std::uint16_t nmi = 0xFFFA;
inline constexpr std::uint16_t reset = 0xFFFC;
inline constexpr std::uint16_t irq = 0xFFFE;
} // namespace vector

struct Registers {
	std::uint8_t a{}; // accumulator register
	std::uint8_t x{}; // index register X
	std::uint8_t y{}; // index register Y
	std::uint8_t s{0xFD}; // stack pointer
	std::uint8_t p{flag::U | flag::I}; // processor status register
	std::uint16_t pc{}; // program counter
};

class Cpu {
public:
	explicit Cpu(Memory& mem) : mem_(mem) {};

	void reset();
	void step();
	void irq();
	void nmi();

	[[nodiscard]] Registers& regs() {
		return r_;
	}
	[[nodiscard]] const Registers& regs() const {
		return r_;
	}
	[[nodiscard]] bool stopped() const {
		return stopped_;
	}
	[[nodiscard]] bool waiting() const {
		return waiting_;
	}

private:
	void execute(std::uint8_t opcode);
	std::uint8_t fetch();
	std::uint16_t fetch_word();
	std::uint8_t pull();
	std::uint16_t pull_word();
	void push(std::uint8_t value);
	void push(std::uint16_t value);
	void interrupt(std::uint16_t vector_address, bool software);

	// Address modes
	std::uint16_t imm();
	std::uint16_t zp();
	std::uint16_t zpx();
	std::uint16_t zpy();
	std::uint16_t abs();
	std::uint16_t absx();
	std::uint16_t absy();
	std::uint16_t zpi();
	std::uint16_t zpxi();
	std::uint16_t zpyi();
	[[nodiscard]] std::uint16_t read_zp_word(std::uint8_t address) const;

	void set_flag(std::uint8_t mask, bool on);
	[[nodiscard]] bool flag_set(std::uint8_t mask) const {return (r_.p & mask) != 0; }
	std::uint8_t set_nz(std::uint8_t value);

	void load(std::uint8_t& reg, std::uint16_t address);
	void store(std::uint8_t value, std::uint16_t address);
	void adc(std::uint8_t value);
	void sbc(std::uint8_t value);
	void compare(std::uint8_t reg, std::uint8_t value);
	void bit(std::uint8_t value, bool immediate);
	void tsb(std::uint16_t address);
	void trb(std::uint16_t address);
	void branch(bool condition);
	void branch_on_bit(unsigned bit, bool set);
	void set_memory_bit(unsigned bit, bool set);

	std::uint8_t asl(std::uint8_t value);
	std::uint8_t lsr(std::uint8_t value);
	std::uint8_t rol(std::uint8_t value);
	std::uint8_t ror(std::uint8_t value);

	template <typename Op>
	void modify(std::uint16_t address, Op op);

	Memory& mem_;
	Registers r_{};
	bool stopped_{};
	bool waiting_{};
};
} // namespace w65c02