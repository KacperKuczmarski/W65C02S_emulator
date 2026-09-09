#include "Memory.h"
#include "Instructions.h"

void Memory::init_mem() {
	for (unsigned char & i : mem) {
		i = NOP;
	}
}
uint8_t Memory::read_byte(const uint16_t Address) const {
	return mem[Address];
}
uint16_t Memory::read_word(const uint16_t Address) const {
	const uint8_t ADL = read_byte(Address);
	const uint8_t ADH = read_byte(Address + 1);
	return ADL | (ADH << 8);
}

void Memory::write_byte(const uint16_t Address, const uint8_t Value) {
	mem[Address] = Value;
}

void Memory::write_test_program_pow2() {
	write_byte(0xFFFC, 0x00); //
	write_byte(0xFFFD, 0x60); // 0x6000 starting vector

	write_byte(0x0000, 0x0000); // write to memory value 1 at address 0x0000

	write_byte(0x6000, LDA_a);  // LOAD to accumulator from memory:
	write_byte(0x6001, 0x00); //
	write_byte(0x6002, 0x00); // 0x0000

	write_byte(0x6003, ADC_a);  // ADd memory to accumulator with Carry
	write_byte(0x6004, 0x00); //
	write_byte(0x6005, 0x00); // 0x0000

	/*
	write_byte(0x6006, TAX);  // Transfer the Accumulator to the X register

	write_byte(0x6007, STA_a);  // STore Accumulator in memory:
	write_byte(0x6008, 0x00); //
	write_byte(0x6009, 0x00); // 0x0000

	write_byte(0x600A, BCS);  // Branch on Carry Set(Pc = 1)
	write_byte(0x600B, 0x04); // to PC + 0x04

	write_byte(0x600C, JMP_a);  // jump to :
	write_byte(0x600D, 0x00); //
	write_byte(0x600E, 0x60); // 0x6000
	*/
	write_byte(0x6006, STP);
}

void Memory::write_test_program_cmp() {
	write_byte(0xFFFC, 0x00); //
	write_byte(0xFFFD, 0x60); // 0x6000 starting vector

	write_byte(0x0000, 0x01); // Write 1 to 0x0000

	write_byte(0x6000, LDY_a); // load value to acc:
	write_byte(0x6001, 0x01);  //
	write_byte(0x6002, 0x00);  // 0x0001

	write_byte(0x6003, CPY_a); // comparte reg X to:
	write_byte(0x6004, 0x00); //
	write_byte(0x6005, 0x00); // to value at 0x0000

	write_byte(0x6006, BEQ); // Branch if equal to:
	write_byte(0x6007, 0x7F); // PC + 127 = (0x6086 = 24 710)
	write_byte(0x600F, STP);
}

void Memory::write_test_substraction() {
	write_byte(0x0001, 0x89);
	write_byte(0x0002, 0x90);

	write_byte(0xFFFC, 0x00); // lower byte
	write_byte(0xFFFD, 0x60); // // higher byte 0x6000 starting vector

	write_byte(0x6000, LDA_a);
	write_byte(0x6001, 0x01); // lower byte
	write_byte(0x6002, 0x00); // // higher byte 0x0001

	write_byte(0x6003, SBC_a);
	write_byte(0x6004, 0x02); // lower byte
	write_byte(0x6005, 0x00); // // higher byte 0x0001

	write_byte(0x600F, STP);

}

void Memory::write_test_temp() {
	write_byte(0x0002, 0x0f); // value 0x0F at 0x0001

	write_byte(0xFFFC, 0x00); // lower byte
	write_byte(0xFFFD, 0x60); // // higher byte 0x6000 starting vector

	write_byte(0x6000, LDA_zp); // Load to A value at address:
	write_byte(0x6001, 0x02); // 0x02
	
	write_byte(0x6002, LDX_ia);
	write_byte(0x6003, 0x01); // 0x01

	write_byte(0x6004, ROR_ax); // Rotate value at address:
	write_byte(0x6005, 0x01); // 0x01
	write_byte(0x6006, 0x00);

	write_byte(0x6007, LDA_zpx); // Load to A value at address:
	write_byte(0x6008, 0x01); // 0x01

	//write_byte(0x6003, ROL_A);
	write_byte(0x6009, NOP);
	write_byte(0x600A, STP);
}

void Memory::write_test_irq() {
	write_byte(0x0001, 0x89);

	write_byte(0xFFFC, 0x00); // lower byte
	write_byte(0xFFFD, 0x60); // // higher byte 0x6000 starting vector

	write_byte(0x6000, LDA_a);
	write_byte(0x6001, 0x01); // lower byte
	write_byte(0x6002, 0x00); // // higher byte 0x0001

	write_byte(0x6003, BRK);


}
