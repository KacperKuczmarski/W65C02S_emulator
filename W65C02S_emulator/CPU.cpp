#include "CPU.h"


void CPU::set_Z_N_flags(uint8_t compare_v) {
	// Zero bit
	P = (compare_v == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
	// Negative bit
	P = (compare_v & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0
}


void CPU::Reset(Memory& mem) {
	PC = mem.readWord(0xFFFC); // load starting vector
	P = 0x24; // 0b0010 0100 (I disabled, BRK, userbit)
	S = 0xFF;
	temp_Byte = 0x00;
}

void CPU::Execute(Memory& mem) {
	// interrupt
	if (P & 0x04 && P & 0x10) {
		PC = 0xFFFE;
	}

	uint8_t inst = mem.read_Byte(PC);

	printf("PC: 0x%x, A: 0x%x, P: 0x%x\n", PC, A, P);

	PC++;

	switch (inst) {
		// ============================================================================
		// ADd memory to accumulator with Carry  
		// ============================================================================
	case ADC_a:
		temp_Byte = A + mem.read_Byte(mem.readWord(PC)) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(mem.readWord(PC)) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(mem.readWord(PC)) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC += 2;
		break;

	case ADC_ia:
		temp_Byte = A + mem.read_Byte(PC) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(PC) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(PC) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC++;
		break;

	case ADC_ax:
		temp_Byte = A + mem.read_Byte(mem.readWord(PC) + X) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(mem.readWord(PC) + X) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(mem.readWord(PC) + X) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC += 2;
		break;

	case ADC_ay:
		temp_Byte = A + mem.read_Byte(mem.readWord(PC) + Y) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(mem.readWord(PC) + Y) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(mem.readWord(PC) + Y) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC += 2;
		break;

	case ADC_zp:
		temp_Byte = A + mem.read_Byte(mem.read_Byte(PC)) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(mem.read_Byte(PC)) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(mem.read_Byte(PC)) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC++;
		break;

	case ADC_zpx:
		temp_Byte = A + mem.read_Byte(mem.read_Byte(PC) + X) + (P & 0x01); // Instruction readWord adds 2 to PC + Carry (P & 0x01)

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A + (uint16_t)mem.read_Byte(mem.read_Byte(PC) + X) + (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A + mem.read_Byte(mem.read_Byte(PC) + X) + (P & 0x01))) ? P | 0x40 : P & 0xBF;
		set_Z_N_flags(temp_Byte);

		A = temp_Byte;
		PC++;
		break;

		// ============================================================================
		// "AND" memory with Accumulator  
		// ============================================================================
	case AND_a:
		A &= mem.read_Byte(mem.readWord(PC));
		set_Z_N_flags(A);
		PC += 2;
		break;

	case AND_ia:
		A &= mem.read_Byte(PC);
		set_Z_N_flags(A);
		PC++;
		break;

	case AND_ax:
		A &= mem.read_Byte(mem.readWord(PC) + X);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case AND_ay:
		A &= mem.read_Byte(mem.readWord(PC) + Y);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case AND_zp:
		A &= mem.read_Byte(mem.read_Byte(PC));
		set_Z_N_flags(A);
		PC += 2;
		break;

	case AND_zpx:
		A &= mem.read_Byte(mem.read_Byte(PC) + X);
		set_Z_N_flags(A);
		PC += 2;
		break;

		// ============================================================================
		// Arithmetic Shift one bit Left, memory or accumulator   
		// ============================================================================
	case ASL_A:
		P = (A & 0x80) ? P | 0x01 : P & 0xFE; // set carry flag if most significant A's bit is set
		A = A << 1; // shift left
		set_Z_N_flags(A);
		break;

	case ASL_zp:
		P = (mem.read_Byte(mem.read_Byte(PC)) & 0x80) ? P | 0x01 : P & 0xFE; // set carry flag if most significant A's bit is set
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) << 1);// shift left
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC)));
		PC++;
		break;

	case ASL_zpx:
		P = (mem.read_Byte(mem.read_Byte(PC) + X) & 0x80) ? P | 0x01 : P & 0xFE; // set carry flag if most significant A's bit is set
		mem.write_Byte(mem.read_Byte(PC) + X, mem.read_Byte(mem.read_Byte(PC) + X) << 1);// shift left
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC) + X));
		PC++;
		break;

		// ============================================================================
		// LoaD Accumulator with memory  
		// ============================================================================
	case LDA_a:
		A = mem.read_Byte(mem.readWord(PC));
		set_Z_N_flags(A);
		PC += 2;
		break;

	case LDA_ia:
		A = mem.read_Byte(PC);
		set_Z_N_flags(A);
		PC++;
		break;

	case LDA_ax:
		A = mem.read_Byte(mem.readWord(PC) + X);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case LDA_ay:
		A = mem.read_Byte(mem.readWord(PC) + Y);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case LDA_zp:
		A = mem.read_Byte(mem.read_Byte(PC));
		set_Z_N_flags(A);
		PC++;
		break;

	case LDA_zpx:
		A = mem.read_Byte(mem.read_Byte(PC) + X);
		set_Z_N_flags(A);
		PC++;
		break;

		// ============================================================================
		// LoaD the X register with memory 
		// ============================================================================
	case LDX_a:
		X = mem.read_Byte(mem.readWord(PC));
		set_Z_N_flags(X);
		PC += 2;
		break;

	case LDX_ia:
		X = mem.read_Byte(PC);
		set_Z_N_flags(X);
		PC++;
		break;

	case LDX_ay:
		X = mem.read_Byte(mem.readWord(PC) + Y);
		set_Z_N_flags(X);
		PC += 2;
		break;

	case LDX_zp:
		X = mem.read_Byte(mem.read_Byte(PC));
		set_Z_N_flags(X);
		PC++;
		break;

	case LDX_zpy:
		X = mem.read_Byte(mem.read_Byte(PC) + Y);
		set_Z_N_flags(X);
		PC++;
		break;

		// ============================================================================
		// LoaD the Y register with memory 
		// ============================================================================
	case LDY_a:
		Y = mem.read_Byte(mem.readWord(PC));
		set_Z_N_flags(Y);
		PC += 2;
		break;

	case LDY_ia:
		Y = mem.read_Byte(PC); // next byte is the value directly for Y register
		set_Z_N_flags(Y);
		PC++;
		break;

	case LDY_ax:
		Y = mem.read_Byte(mem.readWord(PC) + X); // next byte is the value directly for Y register
		set_Z_N_flags(Y);
		PC += 2;
		break;

	case LDY_zp:
		Y = mem.read_Byte(mem.read_Byte(PC));
		set_Z_N_flags(Y);
		PC++;
		break;

	case LDY_zpx:
		Y = mem.read_Byte(mem.read_Byte(PC) + X);
		set_Z_N_flags(Y);
		PC++;
		break;

		// ============================================================================
		// "OR" memory with Accumulator
		// ============================================================================
	case ORA_a:
		A |= mem.read_Byte(mem.readWord(PC));
		PC += 2;

		set_Z_N_flags(A);
		break;

	case ORA_ia:
		A |= mem.read_Byte(PC);
		PC++;

		set_Z_N_flags(A);
		break;

	case ORA_ax:
		A |= mem.read_Byte(mem.readWord(PC) + X);
		PC += 2;

		set_Z_N_flags(A);
		break;

	case ORA_ay:
		A |= mem.read_Byte(mem.readWord(PC) + Y);
		PC += 2;

		set_Z_N_flags(A);
		break;

	case ORA_zp:
		A |= mem.read_Byte(mem.read_Byte(PC));
		PC++;

		set_Z_N_flags(A);
		break;

	case ORA_zpx:
		A |= mem.read_Byte(mem.read_Byte(PC) + X);
		PC++;

		set_Z_N_flags(A);
		break;

		// ============================================================================
		// JuMP to new location
		// ============================================================================
	case JMP_a:
		PC = mem.readWord(PC);
		break;

	case JMP_ai:
		PC = mem.readWord(mem.readWord(PC)); // PC is the address pointed by the PC+1
		break;

		// ============================================================================
		// STore Accumulator in memory  
		// ============================================================================
	case STA_a:
		mem.write_Byte(mem.readWord(PC), A);
		PC += 2;
		break;

	case STA_ax:
		mem.write_Byte(mem.readWord(PC) + X, A);
		PC += 2;
		break;

	case STA_ay:
		mem.write_Byte(mem.readWord(PC) + Y, A);
		PC += 2;
		break;

	case STA_zp:
		mem.write_Byte(mem.read_Byte(PC), A);
		PC++;
		break;

	case STA_zpx:
		mem.write_Byte(mem.read_Byte(PC) + X, A);
		PC++;
		break;

	case TXA:
		A = X;
		break;

		// not sure about the stack pointer
	case TXS:
		S = X;
		break;

		// ============================================================================
		// Test and Reset memory Bit 
		// ============================================================================
	case TRB_zp:
		temp_Byte = A & mem.read_Byte(mem.read_Byte(PC));
		mem.write_Byte(mem.read_Byte(mem.read_Byte(PC)), temp_Byte);
		// Zero bit
		P = (temp_Byte == 0x00) ? P | 0x02 : P & 0xFD; // if res = 0 set Z to 1, else 0 
		PC++;
		break;

		// ============================================================================
		// Test and Set memory Bit 
		// ============================================================================
	case TSB_zp:
		temp_Byte = A | mem.read_Byte(mem.read_Byte(PC));
		mem.write_Byte(mem.read_Byte(mem.read_Byte(PC)), temp_Byte);
		// Zero bit
		P = (temp_Byte == 0x00) ? P | 0x02 : P & 0xFD; // if res = 0 set Z to 1, else 0 
		PC++;
		break;

	case TSX:
		X = S;
		break;

	case TAX:
		X = A;
		break;

	case TYA:
		A = Y;
		break;

	case TAY:
		Y = A;
		break;

	case BBR0:
		PC = (mem.read_Byte(PC) & 0x01) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR1:
		PC = (mem.read_Byte(PC) & 0x02) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR2:
		PC = (mem.read_Byte(PC) & 0x04) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR3:
		PC = (mem.read_Byte(PC) & 0x08) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR4:
		PC = (mem.read_Byte(PC) & 0x10) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR5:
		PC = (mem.read_Byte(PC) & 0x20) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR6:
		PC = (mem.read_Byte(PC) & 0x40) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBR7:
		PC = (mem.read_Byte(PC) & 0x80) ? PC + 1 : PC + int8_t(mem.read_Byte(PC + 1));
		break;

	case BBS0:
		PC = (mem.read_Byte(PC) & 0x01) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS1:
		PC = (mem.read_Byte(PC) & 0x02) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS2:
		PC = (mem.read_Byte(PC) & 0x04) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS3:
		PC = (mem.read_Byte(PC) & 0x08) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS4:
		PC = (mem.read_Byte(PC) & 0x10) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS5:
		PC = (mem.read_Byte(PC) & 0x20) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS6:
		PC = (mem.read_Byte(PC) & 0x40) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BBS7:
		PC = (mem.read_Byte(PC) & 0x80) ? PC + int8_t(mem.read_Byte(PC + 1)) : PC + 1;
		break;

	case BCC:
		PC = (P & 0x01) ? PC + 1 : PC + int8_t(mem.read_Byte(PC));
		break;

	case BCS:
		PC = (P & 0x01) ? PC + int8_t(mem.read_Byte(PC)) : PC + 1;
		break;

	case BEQ:
		PC = (P & 0x02) ? PC + int8_t(mem.read_Byte(PC)) : PC + 1;
		break;

		// ============================================================================
		// BIt Test 
		// ============================================================================
	case BIT_a:
		// set Processor Status register:

		// Negative bit
		P = (mem.read_Byte(mem.readWord(PC)) & 0x80) ? P | 0x80 : P & 0x7F;
		// Overflow bit
		P = (mem.read_Byte(mem.readWord(PC)) & 0x40) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((mem.read_Byte(mem.readWord(PC)) & A) == 0x00) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 

		PC += 2;
		break;

	case BIT_ia:
		// set Processor Status register:

		// Negative bit
		P = (mem.read_Byte(PC) & 0x80) ? P | 0x80 : P & 0x7F;
		// Overflow bit
		P = (mem.read_Byte(PC) & 0x40) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((mem.read_Byte(PC) & A) == 0x00) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 

		PC++;
		break;

	case BIT_ax:
		// set Processor Status register:

		// Negative bit
		P = (mem.read_Byte(mem.readWord(PC) + X) & 0x80) ? P | 0x80 : P & 0x7F;
		// Overflow bit
		P = (mem.read_Byte(mem.readWord(PC) + X) & 0x40) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((mem.read_Byte(mem.readWord(PC) + X) & A) == 0x00) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 

		PC += 2;
		break;

	case BIT_zp:
		// set Processor Status register:

		// Negative bit
		P = (mem.read_Byte(mem.read_Byte(PC)) & 0x80) ? P | 0x80 : P & 0x7F;
		// Overflow bit
		P = (mem.read_Byte(mem.read_Byte(PC)) & 0x40) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((mem.read_Byte(mem.read_Byte(PC)) & A) == 0x00) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 

		PC++;
		break;

	case BIT_zpx:
		// set Processor Status register:

		// Negative bit
		P = (mem.read_Byte(mem.read_Byte(PC) + X) & 0x80) ? P | 0x80 : P & 0x7F;
		// Overflow bit
		P = (mem.read_Byte(mem.read_Byte(PC) + X) & 0x40) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((mem.read_Byte(mem.read_Byte(PC) + X) & A) == 0x00) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 

		PC++;
		break;

	case BMI:
		PC = (P & 0x80) ? PC + int8_t(mem.read_Byte(PC)) : PC + 1;
		break;

	case BNE:
		PC = (P & 0x02) ? PC + 1 : PC + int8_t(mem.read_Byte(PC));
		break;

	case BPL:
		PC = (P & 0x80) ? PC + 1 : PC + int8_t(mem.read_Byte(PC));
		break;

	case BRA:
		PC = PC + int8_t(mem.read_Byte(PC));
		break;

	case BRK:
		// set Processor Status register:
		// Interupt bit
		P |= 0x04;
		//Break bit
		P |= 0x10;

		PC += 2;

		// Write current PC into the stack
		mem.write_Byte(uint16_t(S), (PC + 2) >> 8); // HIGH Byte
		S--;
		mem.write_Byte(uint16_t(S), uint8_t(PC + 2)); // LOW Byte
		S--;
		// Write P into the stack
		mem.write_Byte(uint16_t(S), uint8_t(PC));
		S--;

		break;

	case BVC:
		PC = (P & 0x40) ? PC + 1 : PC + int8_t(mem.read_Byte(PC));
		break;

	case BVS:
		PC = (P & 0x40) ? PC + int8_t(mem.read_Byte(PC)) : PC + 1;
		break;

	case CLC:
		P &= 0xFE;
		break;

	case CLD:
		P &= 0xF7;
		break;

	case CLI:
		P &= 0xFB;
		break;

	case CLV:
		P &= 0xBF;
		break;

		// ============================================================================
		// CoMPare memory and accumulator
		// ============================================================================
	case CMP_a:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(mem.readWord(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case CMP_ia:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(PC);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(PC))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

	case CMP_ax:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(mem.readWord(PC) + X);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC) + X))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case CMP_ay:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(mem.readWord(PC) + Y);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC) + Y))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case CMP_zp:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(mem.read_Byte(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.read_Byte(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

	case CMP_zpx:
		// A - mem: P -> N, Z, C 
		temp_Byte = A - mem.read_Byte(mem.read_Byte(PC) + X);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.read_Byte(PC) + X))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

		// ============================================================================
		// ComPare memory and X register
		// ============================================================================
	case CPX_a:
		// X - mem: P -> N, Z, C 
		temp_Byte = X - mem.read_Byte(mem.readWord(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)X - (uint16_t)mem.read_Byte(mem.readWord(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case CPX_ia:
		// X - mem: P -> N, Z, C 
		temp_Byte = X - mem.read_Byte(PC);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)X - (uint16_t)mem.read_Byte(PC))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

	case CPX_zp:
		// X - mem: P -> N, Z, C 
		temp_Byte = X - mem.read_Byte(mem.read_Byte(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)X - (uint16_t)mem.read_Byte(mem.read_Byte(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

		// ============================================================================
		// ComPare memory and Y register 
		// ============================================================================
	case CPY_a:
		// Y - mem: P -> N, Z, C 
		temp_Byte = Y - mem.read_Byte(mem.readWord(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)Y - (uint16_t)mem.read_Byte(mem.readWord(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(A);

		PC += 2;
		break;

	case CPY_ia:
		// Y - mem: P -> N, Z, C 
		temp_Byte = Y - mem.read_Byte(PC);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)Y - (uint16_t)mem.read_Byte(PC))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

	case CPY_zp:
		// Y - mem: P -> N, Z, C 
		temp_Byte = Y - mem.read_Byte(mem.read_Byte(PC));

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)Y - (uint16_t)mem.read_Byte(mem.read_Byte(PC)))) ? P | 0x01 : P & 0xFE;
		set_Z_N_flags(temp_Byte);

		PC++;
		break;

		// ============================================================================
		// DECrement memory or accumulate by one 
		// ============================================================================
	case DEC_A:
		A--;
		set_Z_N_flags(A);
		break;

	case DEC_a:
		mem.write_Byte(mem.readWord(PC), mem.read_Byte(mem.readWord(PC)) - 1);
		set_Z_N_flags(A);

		PC += 2;
		break;

	case DEC_ax:
		mem.write_Byte(mem.readWord(PC) + X, mem.read_Byte(mem.readWord(PC) + X) - 1);
		set_Z_N_flags(mem.read_Byte(mem.readWord(PC) + X));

		PC += 2;
		break;

	case DEC_zp:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) - 1);
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC)));

		PC++;
		break;

	case DEC_zpx:
		mem.write_Byte(mem.read_Byte(PC) + X, mem.read_Byte(mem.read_Byte(PC) + X) - 1);
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC) + X));

		PC++;
		break;

	case DEX:
		X--;
		set_Z_N_flags(A);
		break;

	case DEY:
		Y--;
		set_Z_N_flags(A);
		break;

		// ============================================================================
		// "Exclusive OR" memory with accumulate
		// ============================================================================
	case EOR_a:
		A = A ^ mem.read_Byte(mem.readWord(PC));
		set_Z_N_flags(A);
		PC += 2;
		break;

	case EOR_ia:
		A = A ^ mem.read_Byte(PC);
		set_Z_N_flags(A);
		PC++;
		break;

	case EOR_ax:
		A = A ^ mem.read_Byte(mem.readWord(PC) + X);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case EOR_ay:
		A = A ^ mem.read_Byte(mem.readWord(PC) + Y);
		set_Z_N_flags(A);
		PC += 2;
		break;

	case EOR_zp:
		A = A ^ mem.read_Byte(mem.read_Byte(PC));
		set_Z_N_flags(A);
		PC++;
		break;

	case EOR_zpx:
		A = A ^ mem.read_Byte(mem.read_Byte(PC) + X);
		set_Z_N_flags(A);
		PC++;
		break;

		// ============================================================================
		// INCrement memory or accumulate by one
		// ============================================================================
	case INC_A:
		A++;
		set_Z_N_flags(A);
		break;

	case INC_a:
		mem.write_Byte(mem.readWord(PC), mem.read_Byte(mem.readWord(PC)) + 1);
		set_Z_N_flags(mem.read_Byte(mem.readWord(PC)));
		PC += 2;
		break;

	case INC_ax:
		mem.write_Byte(mem.readWord(PC) + X, mem.read_Byte(mem.readWord(PC) + X) + 1);
		set_Z_N_flags(mem.read_Byte(mem.readWord(PC) + X));
		PC += 2;
		break;

	case INC_zp:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) + 1);
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC)));
		PC++;
		break;

	case INC_zpx:
		mem.write_Byte(mem.read_Byte(PC) + X, mem.read_Byte(mem.read_Byte(PC) + X) + 1);
		set_Z_N_flags(mem.read_Byte(mem.read_Byte(PC) + X));
		PC++;
		break;

	case INX:
		X++;
		mem.read_Byte(X);
		break;

	case INY:
		Y++;
		mem.read_Byte(Y);
		break;

	case JSR:
		// Write current PC into the stack
		mem.write_Byte(uint16_t(S), (PC + 2) >> 8); // HIGH Byte
		S--;
		mem.write_Byte(uint16_t(S), uint8_t(PC + 2)); // LOW Byte
		S--;

		PC = mem.readWord(PC);
		break;

	case RTS:
		// Read current PC from the stack
		PC = mem.readWord(S + 1);
		S += 2;

		break;

	case RTI:
		P = mem.read_Byte(S);
		S++;
		// Read current PC from the stack
		PC = mem.readWord(S);
		S += 2;
		break;

		// ============================================================================
		// Logical Shift one bit Right memory or accumulator
		// ============================================================================
	case LSR_A:
		// set Carry bit first 
		P = (A & 0x01) ? P | 0x01 : P & 0xFE;
		A = A >> 1;
		// set Processor Status register:

		// Zero bit
		P = P & 0xFD; // always clear Zero flag
		// Negative bit
		P = P & 0x7F; // always clear Negative flag
		break;

	case LSR_a:
		// set Carry bit first 
		P = (mem.read_Byte(mem.readWord(PC)) & 0x01) ? P | 0x01 : P & 0xFE;
		mem.write_Byte(mem.readWord(PC), mem.read_Byte(mem.readWord(PC)) >> 1);
		// set Processor Status register:

		// Zero bit
		P = (mem.read_Byte(mem.readWord(PC)) == 0) ? P | 0x02 : P & 0xFD; // always clear Zero flag
		// Negative bit
		P = P & 0x7F; // always clear Negative flag

		PC += 2;
		break;

	case LSR_ax:
		// set Carry bit first 
		P = (mem.read_Byte(mem.readWord(PC) + X) & 0x01) ? P | 0x01 : P & 0xFE;
		mem.write_Byte(mem.readWord(PC) + X, mem.read_Byte(mem.readWord(PC) + X) >> 1);
		// set Processor Status register:

		// Zero bit
		P = (mem.read_Byte(mem.readWord(PC) + X) == 0) ? P | 0x02 : P & 0xFD; // Zero flag
		// Negative bit
		P = P & 0x7F; // always clear Negative flag

		PC += 2;
		break;

	case LSR_zp:
		// set Carry bit first 
		P = (mem.read_Byte(mem.read_Byte(PC)) & 0x01) ? P | 0x01 : P & 0xFE;
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) >> 1);
		// set Processor Status register:

		// Zero bit
		P = (mem.read_Byte(mem.read_Byte(PC)) == 0) ? P | 0x02 : P & 0xFD; // always clear Zero flag
		// Negative bit
		P = P & 0x7F; // always clear Negative flag

		PC += 2;
		break;

	case LSR_zpx:
		// set Carry bit first 
		P = (mem.read_Byte(mem.read_Byte(PC) + X) & 0x01) ? P | 0x01 : P & 0xFE;
		mem.write_Byte(mem.read_Byte(PC) + X, mem.read_Byte(mem.read_Byte(PC) + X) >> 1);
		// set Processor Status register:

		// Zero bit
		P = (mem.read_Byte(mem.read_Byte(PC) + X) == 0) ? P | 0x02 : P & 0xFD; // always clear Zero flag
		// Negative bit
		P = P & 0x7F; // always clear Negative flag

		PC += 2;
		break;

	case NOP:
		// do nothing :)
		break;

	case PHA:
		mem.write_Byte(S, A);
		S--;
		break;

	case PHP:
		mem.write_Byte(S, P);
		S--;
		break;

	case PHX:
		mem.write_Byte(S, X);
		S--;
		break;

	case PHY:
		mem.write_Byte(S, Y);
		S--;
		break;

	case PLA:
		A = mem.read_Byte(S);
		S++;
		set_Z_N_flags(A);
		break;

	case PLP:
		P = mem.read_Byte(S);
		S++;
		break;

	case PLX:
		X = mem.read_Byte(S);
		S++;
		mem.read_Byte(X);
		break;

	case PLY:
		Y = mem.read_Byte(S);
		S++;
		mem.read_Byte(Y);
		break;

	case RMB0:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xFE); // set 0 bit to zero
		PC++;
		break;

	case RMB1:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xFD); // set 1 bit to zero
		PC++;
		break;

	case RMB2:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xFB); // set 2 bit to zero
		PC++;
		break;

	case RMB3:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xF7); // set 3 bit to zero
		PC++;
		break;

	case RMB4:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xEF); // set 4 bit to zero
		PC++;
		break;

	case RMB5:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xDF); // set 5 bit to zero
		PC++;
		break;

	case RMB6:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0xBF); // set 6 bit to zero
		PC++;
		break;

	case RMB7:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) & 0x7F); // set 7 bit to zero
		PC++;
		break;

	case SMB0:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x01); // set 0 bit to one
		PC++;
		break;

	case SMB1:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x02); // set 1 bit to one
		PC++;
		break;

	case SMB2:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x04); // set 2 bit to one
		PC++;
		break;

	case SMB3:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x08); // set 3 bit to one
		PC++;
		break;

	case SMB4:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x10); // set 4 bit to one
		PC++;
		break;

	case SMB5:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x20); // set 5 bit to one
		PC++;
		break;

	case SMB6:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x40); // set 6 bit to one
		PC++;
		break;

	case SMB7:
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) | 0x80); // set 7 bit to one
		PC++;
		break;

		// ============================================================================
		// ROtate one bit Left memory or accumulator 
		// ============================================================================
	case ROL_A:
		P |= (A & 0x80) >> 7; // set carry flag if most significant A's bit is set
		A = A << 1; // shift left
		A |= P & 0x01; // add carry to least significant A's bit 
		set_Z_N_flags(A);
		break;

	case ROL_a:
		P |= (mem.read_Byte(mem.readWord(PC)) & 0x80) >> 7; // set carry flag if most significant value's bit is set
		mem.write_Byte(mem.readWord(PC), mem.read_Byte(mem.readWord(PC)) << 1); // shift left
		temp_Byte = mem.read_Byte(mem.readWord(PC)) | P & 0x01; // calculate byte with carry
		mem.write_Byte(mem.readWord(PC), temp_Byte); // put the final result to the memory

		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case ROL_ax:
		P |= (mem.read_Byte(mem.readWord(PC) + X) & 0x80) >> 7; // set carry flag if most significant value's bit is set
		mem.write_Byte(mem.readWord(PC) + X, mem.read_Byte(mem.readWord(PC) + X) << 1); // shift left
		temp_Byte = mem.read_Byte(mem.readWord(PC) + X) | P & 0x01; // calculate byte with carry
		mem.write_Byte(mem.readWord(PC) + X, temp_Byte); // put the final result to the memory

		set_Z_N_flags(temp_Byte);

		PC += 2;
		break;

	case ROL_zp:
		P |= (mem.read_Byte(mem.read_Byte(PC)) & 0x80) >> 7; // set carry flag if most significant value's bit is set
		mem.write_Byte(mem.read_Byte(PC), mem.read_Byte(mem.read_Byte(PC)) << 1); // shift left
		temp_Byte = mem.read_Byte(mem.read_Byte(PC)) | P & 0x01; // calculate byte with carry
		mem.write_Byte(mem.read_Byte(PC), temp_Byte); // put the final result to the memory

		set_Z_N_flags(temp_Byte);

		PC++;
		break;

	case ROL_zpx:
		P |= (mem.read_Byte(mem.read_Byte(PC) + X) & 0x80) >> 7; // set carry flag if most significant value's bit is set
		mem.write_Byte(mem.read_Byte(PC) + X, mem.read_Byte(mem.read_Byte(PC) + X) << 1); // shift left
		temp_Byte = mem.read_Byte(mem.read_Byte(PC) + X) | P & 0x01; // calculate byte with carry
		mem.write_Byte(mem.read_Byte(PC) + X, temp_Byte); // put the final result to the memory

		set_Z_N_flags(temp_Byte);

		PC++;
		break;

		// ============================================================================
		// ROtate one bit Right memory or accumulator 
		// ============================================================================
	case ROR_A:
		P |= A & 0x01; // set carry flag if least significant A's bit is set
		A = A >> 1; // shift right
		A |= P & 0x80; // add carry to most significant A's bit 
		set_Z_N_flags(A);
		break;

		// ============================================================================
		// SuBtract memory from accumulator with borrow (Carry bit) 
		// ============================================================================
	case SBC_a:
		temp_Byte = A - mem.read_Byte(mem.readWord(PC)) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC)) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(mem.readWord(PC)) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(mem.readWord(PC)) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(mem.readWord(PC)) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC += 2;
		break;

	case SBC_ia:
		temp_Byte = A - mem.read_Byte(PC) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(PC) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(PC) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(PC) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(PC) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC++;
		break;

	case SBC_ax:
		temp_Byte = A - mem.read_Byte(mem.readWord(PC) + X) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC) + X) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(mem.readWord(PC) + X) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(mem.readWord(PC) + X) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(mem.readWord(PC) + X) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC += 2;
		break;

	case SBC_ay:
		temp_Byte = A - mem.read_Byte(mem.readWord(PC) + Y) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.readWord(PC) + Y) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(mem.readWord(PC) + Y) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(mem.readWord(PC) + Y) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(mem.readWord(PC) + Y) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC += 2;
		break;

	case SBC_zp:
		temp_Byte = A - mem.read_Byte(mem.read_Byte(PC)) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.read_Byte(PC)) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(mem.read_Byte(PC)) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(mem.read_Byte(PC)) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(mem.read_Byte(PC)) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC++;
		break;

	case SBC_zpx:
		temp_Byte = A - mem.read_Byte(mem.read_Byte(PC) + X) - (P & 0x01);

		// set Processor Status register:
		// Carry bit
		P = (UINT8_MAX < uint16_t((uint16_t)A - (uint16_t)mem.read_Byte(mem.read_Byte(PC) + X) - (P & 0x01))) ? P | 0x01 : P & 0xFE;
		// Overflow bit
		P = (UINT8_MAX < uint16_t(A - mem.read_Byte(mem.read_Byte(PC) + X) - (P & 0x01))) ? P | 0x40 : P & 0xBF;
		// Zero bit
		P = ((A - mem.read_Byte(mem.read_Byte(PC) + X) - (P & 0x01)) == 0) ? P | 0x02 : P & 0xFD; // if A = 0 set Z to 1, else 0 
		// Negative bit
		P = ((A - mem.read_Byte(mem.read_Byte(PC) + X) - (P & 0x01)) & 0x80) ? P | 0x80 : P & 0x7F; // if A's last bit is 1 set N to 1, else 0

		A = temp_Byte;
		PC++;
		break;

	case SEC:
		P |= 0x01;
		break;

	case SED:
		P |= 0x08;
		break;

	case SEI:
		P |= 0x04;
		break;

	case STP:
		// it stops CPU until hard reset
		no_further_inst = true;
		break;

		// ============================================================================
		// STore the X register in memory
		// ============================================================================
	case STX_a:
		mem.write_Byte(mem.readWord(PC), X);
		PC += 2;
		break;

	case STX_zp:
		mem.write_Byte(mem.read_Byte(PC), X);
		PC++;
		break;

	case STX_zpy:
		mem.write_Byte(mem.read_Byte(PC) + Y, X);
		PC++;
		break;

		// ============================================================================
		// STore the Y register in memory
		// ============================================================================
	case STY_a:
		mem.write_Byte(mem.readWord(PC), Y);
		PC += 2;
		break;

	case STY_zp:
		mem.write_Byte(mem.read_Byte(PC), Y);
		PC++;
		break;

	case STY_zpx:
		mem.write_Byte(mem.read_Byte(PC) + X, Y);
		PC++;
		break;

		// ============================================================================
		// STore Zero register in memory
		// ============================================================================
	case STZ_a:
		mem.write_Byte(mem.readWord(PC), 0x00);
		PC += 2;
		break;

	case STZ_ax:
		mem.write_Byte(mem.readWord(PC) + X, 0x00);
		PC += 2;
		break;

	case STZ_zp:
		mem.write_Byte(mem.read_Byte(PC), 0x00);
		PC++;
		break;

	case STZ_zpx:
		mem.write_Byte(mem.read_Byte(PC) + X, 0x00);
		PC++;
		break;

	default:
		no_further_inst = 1;
		printf("\nCPU stop\n");
		break;
	}
}


