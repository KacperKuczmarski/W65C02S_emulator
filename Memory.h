#pragma once
#include <cstdint>
#define MAX_ADD_MEM 0x10000

class Memory {
	uint8_t mem[MAX_ADD_MEM]; // all addressable memory

public:
	void init_mem();

	uint8_t read_byte(uint16_t Address) const;

	uint16_t read_word(uint16_t Address) const;

	void write_byte(uint16_t Address, uint8_t Value);

	void write_test_program_pow2();

	void write_test_program_cmp();

	void write_test_substraction();

	void write_test_temp();

	void write_test_irq();
};
