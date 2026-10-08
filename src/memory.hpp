#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <iostream>

namespace w65c02s {
class Memory {
public:
	static constexpr std::size_t size = {0x10000};

	[[nodiscard]] std::uint8_t read(const std::uint16_t address) const {
	    return data_[address];
	}
	[[nodiscard]] std::uint16_t read_word(const std::uint16_t address) const {
	    const uint8_t adl = read(address);
	    const auto adh = static_cast<std::uint16_t>(read(address + 1));
	    return adl | (adh << 8);
	}
	void write(const std::uint16_t address, const std::uint8_t value) {
	    data_[address] = value;
	}
	void write_word(const std::uint16_t address, std::uint16_t value) {
	    write(address, static_cast<std::uint8_t>(value));
	    write(address + 1, static_cast<std::uint8_t>(value >> 8));
	}

	void load(const std::span<const std::uint8_t> data, uint16_t address) {
	    for (const auto byte : data) {
	    	write(address++, byte);
	    }
	}

private:
	std::array<std::uint8_t, size> data_{};
};
} // namespace w65c02
