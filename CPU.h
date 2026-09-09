#pragma once
#include <cstdint>
#include "Memory.h"

class CPU {
	uint8_t A{}; // Accumulator register
	uint8_t X{}; // Index register X
	uint8_t Y{}; // Index register Y
	uint16_t PC{}; // Program Counter
	uint8_t S{}; // Stack Pointer

	uint8_t P{}; // Processor Status Register
	// This status contains:
	// 0 - ( C ) Carry 
	// 1 - ( Z ) Zero
	// 2 - ( I ) IRQB disable
	// 3 - ( D ) Decimal mode
	// 4 - ( B ) BRK command 1 = BRK, 0 = IRQB
	// 5 - ( 1 ) User bit
	// 6 - ( V ) Overflow
	// 7 - ( N ) Negative

	uint8_t temp_Byte{}; // temporary Accumulator for emulation purposes, it is not part of the cpu

	void set_Z_N_flags(uint8_t compare_v);

public:
	uint8_t no_further_inst = 0;

	void reset(Memory& mem);
	void execute(Memory& mem);
};