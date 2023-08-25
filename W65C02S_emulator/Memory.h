#pragma once
#include <cstdint>
#include <stdio.h>
#define MAX_ADD_MEM 0x10000
#include "Instructions.h"

class Memory {
	uint8_t mem[MAX_ADD_MEM]; // all addressable memory

public:
	void init_mem();

	uint8_t read_Byte(uint16_t Address);

	uint16_t readWord(uint16_t Address);

	void write_Byte(uint16_t Address, uint8_t Value);

	void write_test_program_pow2();

	void write_test_program_cmp();

	void write_test_substraction();

	void write_test_temp();

	void write_test_irq();
};
