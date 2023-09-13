#include <cstdint>
#include <stdio.h>

constexpr uint8_t BRK = 0X00; // BReaK instruction
constexpr uint8_t BPL = 0X10; // Branch if result PLus (Pn=0) 
constexpr uint8_t JSR = 0X20; // Jump to new location Saving Return (Jump to SubRoutine) 
constexpr uint8_t BMI = 0X30; // Branch if result MInus (Pn=1) 
constexpr uint8_t RTI = 0X40; // ReTurn from Interrupt 
constexpr uint8_t BVC = 0X50; // Branch on oVerflow Clear (Pv=0) 
constexpr uint8_t RTS = 0X60; // ReTurn from Subroutine
constexpr uint8_t BVS = 0X70; // Branch on oVerflow Set (Pv=1)
constexpr uint8_t BRA = 0X80; // BRanch Always
constexpr uint8_t BCC = 0X90; // Branch on Carry Clear(Pc = 0)
constexpr uint8_t LDY_ia = 0XA0; // LoaD the Y register with memory Immediate Addressing # 
constexpr uint8_t BCS = 0XB0; // Branch on Carry Set(Pc = 1)
constexpr uint8_t CPY_ia = 0XC0; // ComPare memory and Y register Immediate Addressing # 
constexpr uint8_t BNE = 0XD0; // Branch if Not Equal (Pz=0) 
constexpr uint8_t CPX_ia = 0XE0; // ComPare memory and X register Immediate Addressing # 
constexpr uint8_t BEQ = 0XF0; // Branch if EQual (Pz=1) (so if zero bit is set)


constexpr uint8_t ORA_zpxi = 0X01; // "OR" memory with Accumulator Zero Page Indexed Indirect (zp,x) 
constexpr uint8_t ORA_zpyi = 0X11; // "OR" memory with Accumulator Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t AND_zpxi = 0X21; // "AND" memory with Accumulator Zero Page Indexed Indirect (zp,x)
constexpr uint8_t AND_zpyi = 0X31; // "AND" memory with Accumulator Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t EOR_zpxi = 0X41; // "Exclusive OR" memory with accumulate Zero Page Indexed Indirect (zp,x)
constexpr uint8_t EOR_zpyi = 0X51; // "Exclusive OR" memory with accumulate Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t ADC_zpxi = 0X61; // ADd memory to accumulator with Carry Zero Page Indexed Indirect (zp,x)
constexpr uint8_t ADC_zpyi = 0X71; // ADd memory to accumulator with Carry Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t STA_zpxi = 0X81; // STore Accumulator in memory Zero Page Indexed Indirect (zp,x)
constexpr uint8_t STA_zpyi = 0X91; // STore Accumulator in memory Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t LDA_zpxi = 0XA1; // LoaD Accumulator with memory Zero Page Indexed Indirect (zp,x)
constexpr uint8_t LDA_zpyi = 0XB1; // LoaD Accumulator with memory Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t CMP_zpxi = 0XC1; // CoMPare memory and accumulator Zero Page Indexed Indirect (zp,x)
constexpr uint8_t CMP_zpyi = 0XD1; // CoMPare memory and accumulator Zero Page Indirect Indexed with Y (zp), y
constexpr uint8_t SBC_zpxi = 0XE1; // SuBtract memory from accumulator with borrow (Carry bit) Zero Page Indexed Indirect (zp,x)
constexpr uint8_t SBC_zpyi = 0XF1; // SuBtract memory from accumulator with borrow (Carry bit) Zero Page Indirect Indexed with Y (zp), y

constexpr uint8_t ORA_zpi = 0X12; // "OR" memory with Accumulator Zero Page Indirect (zp) 
constexpr uint8_t AND_zpi = 0X32; // "AND" memory with Accumulator Zero Page Indirect (zp) 
constexpr uint8_t EOR_zpi = 0X52; // "Exclusive OR" memory with accumulate Zero Page Indirect (zp)
constexpr uint8_t ADC_zpi = 0X72; // ADd memory to accumulator with Carry Zero Page Indirect (zp)
constexpr uint8_t STA_zpi = 0X92; // STore Accumulator in memory Zero Page Indirect (zp)
constexpr uint8_t LDX_ia = 0XA2; // LoaD the X register with memory Immediate Addressing # 
constexpr uint8_t LDX_zpi = 0XB2; // LoaD the X register with memory Zero Page Indirect (zp)
constexpr uint8_t CMP_zpi = 0XD2; // CoMPare memory and accumulator Zero Page Indirect (zp)
constexpr uint8_t SBC_zpi = 0XF2; // SuBtract memory from accumulator with borrow (Carry bit) Zero Page Indirect (zp)

constexpr uint8_t TSB_zp = 0X04; // Test and Set memory Bit zp Zero Page 
constexpr uint8_t TRB_zp = 0X14; // Test and Reset memory Bit zp Zero Page
constexpr uint8_t BIT_zp = 0X24; // BIt Test zp Zero Page
constexpr uint8_t BIT_zpx = 0X34; //BIt Test zpx Zero Page Indexed with X zp,x 
constexpr uint8_t STZ_zp = 0X64; // STore Zero in memory zp Zero Page
constexpr uint8_t STZ_zpx = 0X74; // STore Zero in memory zpx Zero Page Indexed with X zp,x 
constexpr uint8_t STY_zp = 0X84; // STore the Y register in memory zp Zero Page
constexpr uint8_t STY_zpx = 0X94; // STore the Y register in memory zpx Zero Page Indexed with X zp,x 
constexpr uint8_t LDY_zp = 0XA4; // LoaD the Y register with memory zp Zero Page
constexpr uint8_t LDY_zpx = 0XB4; // LoaD the Y register with memory zpx Zero Page Indexed with X zp,x 
constexpr uint8_t CPY_zp = 0XC4; // ComPare memory and Y register zp Zero Page
constexpr uint8_t CPX_zp = 0XE4; // ComPare memory and X register zp Zero Page


constexpr uint8_t ORA_zp = 0X05; // "OR" memory with Accumulator zp Zero Page
constexpr uint8_t ORA_zpx = 0X15; // "OR" memory with Accumulator zpx Zero Page Indexed with X zp,x 
constexpr uint8_t AND_zp = 0X25; // "AND" memory with Accumulator zp Zero Page
constexpr uint8_t AND_zpx = 0X35; // "AND" memory with Accumulator zpx Zero Page Indexed with X zp,x 
constexpr uint8_t EOR_zp = 0X45; // "Exclusive OR" memory with accumulate zp Zero Page
constexpr uint8_t EOR_zpx = 0X55; // "Exclusive OR" memory with accumulate zpx Zero Page Indexed with X zp,x 
constexpr uint8_t ADC_zp = 0X65; // ADd memory to accumulator with Carry zp Zero Page
constexpr uint8_t ADC_zpx = 0X75; // ADd memory to accumulator with Carry zpx Zero Page indexed with X zp, x
constexpr uint8_t STA_zp = 0X85; // STore Accumulator in memory zp Zero Page
constexpr uint8_t STA_zpx = 0X95; // STore Accumulator in memory zpx Zero Page indexed with X zp, x
constexpr uint8_t LDA_zp = 0XA5; // LoaD Accumulator with memory zp Zero Page
constexpr uint8_t LDA_zpx = 0XB5; // LoaD Accumulator with memory zpx Zero Page indexed with X zp, x
constexpr uint8_t CMP_zp = 0XC5; // CoMPare memory and accumulator zp Zero Page
constexpr uint8_t CMP_zpx = 0XD5; // CoMPare memory and accumulator zpx Zero Page indexed with X zp, x
constexpr uint8_t SBC_zp = 0XE5; // SuBtract memory from accumulator with borrow (Carry bit) zp Zero Page
constexpr uint8_t SBC_zpx = 0XF5; // SuBtract memory from accumulator with borrow (Carry bit) indexed with X zp, x

constexpr uint8_t ASL_zp = 0X06; // Arithmetic Shift one bit Left, memory or accumulator zp Zero Page
constexpr uint8_t ASL_zpx = 0X16; // Arithmetic Shift one bit Left, memory or accumulator zpx Zero Page indexed with X zp, x
constexpr uint8_t ROL_zp = 0X26; // ROtate one bit Left memory or accumulator zp Zero Page
constexpr uint8_t ROL_zpx = 0X36; // ROtate one bit Left memory or accumulator zpx Zero Page indexed with X zp, x
constexpr uint8_t LSR_zp = 0X46; // Logical Shift one bit Right memory or accumulator zp Zero Page
constexpr uint8_t LSR_zpx = 0X56; // Logical Shift one bit Right memory or accumulator zpx Zero Page indexed with X zp, x
constexpr uint8_t ROR_zp = 0X66; // ROtate one bit Right memory or accumulator zp Zero Page
constexpr uint8_t ROR_zpx = 0X76; // ROtate one bit Right memory or accumulator zpx Zero Page indexed with X zp, x
constexpr uint8_t STX_zp = 0X86; // STore the X register in memory zp Zero Page
constexpr uint8_t STX_zpy = 0X96; // STore the X register in memory zpx Zero Page indexed with Y zp, y
constexpr uint8_t LDX_zp = 0XA6; // LoaD the X register with memory zp Zero Page
constexpr uint8_t LDX_zpy = 0XB6; // LoaD the X register with memory zpx Zero Page indexed with Y zp, y
constexpr uint8_t DEC_zp = 0XC6; // DECrement memory or accumulate by one zp Zero Page
constexpr uint8_t DEC_zpx = 0XD6; // DECrement memory or accumulate by one zpx Zero Page indexed with X zp, x
constexpr uint8_t INC_zp = 0XE6; // INCrement memory or accumulate by one zp Zero Page
constexpr uint8_t INC_zpx = 0XF6; // INCrement memory or accumulate by one zpx Zero Page indexed with X zp, x

constexpr uint8_t RMB0 = 0X07; // Reset Memory Bit 0
constexpr uint8_t RMB1 = 0X17; // Reset Memory Bit 1
constexpr uint8_t RMB2 = 0X27; // Reset Memory Bit 2
constexpr uint8_t RMB3 = 0X37; // Reset Memory Bit 3
constexpr uint8_t RMB4 = 0X47; // Reset Memory Bit 4
constexpr uint8_t RMB5 = 0X57; // Reset Memory Bit 5
constexpr uint8_t RMB6 = 0X67; // Reset Memory Bit 6
constexpr uint8_t RMB7 = 0X77; // Reset Memory Bit 7
constexpr uint8_t SMB0 = 0X87; // Set Memory Bit 0
constexpr uint8_t SMB1 = 0X97; // Set Memory Bit 1
constexpr uint8_t SMB2 = 0XA7; // Set Memory Bit 2
constexpr uint8_t SMB3 = 0XB7; // Set Memory Bit 3
constexpr uint8_t SMB4 = 0XC7; // Set Memory Bit 4
constexpr uint8_t SMB5 = 0XD7; // Set Memory Bit 5
constexpr uint8_t SMB6 = 0XE7; // Set Memory Bit 6
constexpr uint8_t SMB7 = 0XF7; // Set Memory Bit 7

constexpr uint8_t PHP = 0X08; // PusH Processor status on stack
constexpr uint8_t CLC = 0X18; // CLear Cary flag 
constexpr uint8_t PLP = 0X28; // PuLl Processor status from stack
constexpr uint8_t SEC = 0X38; // SEt Carry 
constexpr uint8_t PHA = 0X48; // PusH Accumulator on stack 
constexpr uint8_t CLI = 0X58; // CLear Interrupt disable bit 
constexpr uint8_t PLA = 0X68; // PuLl Accumulator from stack 
constexpr uint8_t SEI = 0X78; // SEt Interrupt disable status 
constexpr uint8_t DEY = 0X88; // DEcrement Y by one
constexpr uint8_t TYA = 0X98; // Transfer Y register to the Accumulator
constexpr uint8_t TAY = 0XA8; // Transfer the Accumulator to the Y register
constexpr uint8_t CLV = 0XB8; // CLear oVerflow flag
constexpr uint8_t INY = 0XC8; // INcrement Y register by one
constexpr uint8_t CLD = 0XD8; // CLear Decimal mode 
constexpr uint8_t INX = 0XE8; // INcrement X register by one
constexpr uint8_t SED = 0XF8; // SEt Decimal mode


constexpr uint8_t ORA_ia = 0X09; // "OR" memory with Accumulator Immediate Addressing # 
constexpr uint8_t ORA_ay = 0X19; // "OR" memory with Accumulator a,y Absolute Indexed with Y
constexpr uint8_t AND_ia = 0X29; // "AND" memory with accumulator Immediate Addressing # 
constexpr uint8_t AND_ay = 0X39; // "AND" memory with accumulator a,y Absolute Indexed with Y 
constexpr uint8_t EOR_ia = 0X49; // "Exclusive OR" memory with accumulate Immediate Addressing # 
constexpr uint8_t EOR_ay = 0X59; // "Exclusive OR" memory with accumulate a,y Absolute Indexed with Y 
constexpr uint8_t ADC_ia = 0X69; // ADd memory to accumulator with Carry  # Immediate
constexpr uint8_t ADC_ay = 0X79; // ADd memory to accumulator with Carry  a,y Absolute Indexed with Y
constexpr uint8_t BIT_ia = 0X89; // BIt Test Immediate Addressing # 
constexpr uint8_t STA_ay = 0X99; // STore Accumulator in memory a,y Absolute Indexed with Y
constexpr uint8_t LDA_ia = 0XA9; // LoaD Accumulator with memory Immediate Addressing # 
constexpr uint8_t LDA_ay = 0XB9; // LoaD Accumulator with memory a,y Absolute Indexed with Y 
constexpr uint8_t CMP_ia = 0XC9; // CoMPare memory and accumulator Immediate Addressing # 
constexpr uint8_t CMP_ay = 0XD9; // CoMPare memory and accumulator a,y Absolute Indexed with Y
constexpr uint8_t SBC_ia = 0XE9; // SuBtract memory from accumulator with borrow (Carry bit) Immediate Addressing #
constexpr uint8_t SBC_ay = 0XF9; // SuBtract memory from accumulator with borrow (Carry bit) a,y Absolute Indexed with Y

constexpr uint8_t ASL_A = 0X0A; // Arithmetic Shift one bit Left, memory or accumulator A Accumulator
constexpr uint8_t INC_A = 0X1A; // INCrement memory or accumulate by one A Accumulator
constexpr uint8_t ROL_A = 0X2A; // ROtate one bit Left memory or accumulator A Accumulator
constexpr uint8_t DEC_A = 0X3A; // DECrement memory or accumulate by one A Accumulator
constexpr uint8_t LSR_A = 0X4A; // Logical Shift one bit Right memory or accumulator A Accumulator
constexpr uint8_t PHY = 0X5A; // PusH Y register on stack
constexpr uint8_t ROR_A = 0X6A; // ROtate one bit Right memory or accumulator A Accumulator
constexpr uint8_t PLY = 0X7A; // PuLl Y register from stack
constexpr uint8_t TXA = 0X8A; // Transfer the X register to the Accumulator
constexpr uint8_t TXS = 0X9A; // Transfer the X register to the Stack pointer  register
constexpr uint8_t TAX = 0XAA; // Transfer the Accumulator to the X register
constexpr uint8_t TSX = 0XBA; // Transfer the Stack pointer to the X register 
constexpr uint8_t DEX = 0XCA; // DEcrement X by one
constexpr uint8_t PHX = 0XDA; // PusH X register on stack
constexpr uint8_t NOP = 0XEA; // No OPeration 
constexpr uint8_t PLX = 0XFA; // PuLl X register from stack 

constexpr uint8_t WAI = 0XCB; // WAit for Interrupt
constexpr uint8_t STP = 0XDB; // SToP mode

constexpr uint8_t TSB_a = 0X0C; // Test and Set memory Bit a Absolute
constexpr uint8_t TRB_a = 0X1C; // Test and Reset memory Bit a Absolute
constexpr uint8_t BIT_a = 0X2C; // BIt Test a Absoulute
constexpr uint8_t BIT_ax = 0X3C; //BIt Test Absolute Indexed with X a,x
constexpr uint8_t JMP_a = 0X4C; // JuMP to new location a Absolute
constexpr uint8_t JMP_ai = 0X6C; // JuMP to new location (a) Absolute Indirect
constexpr uint8_t JMP_axi = 0X7C; // JuMP to new location
constexpr uint8_t STY_a = 0X8C; // STore the Y register in memory a Absolute
constexpr uint8_t STZ_a = 0X9C; // STore Zero in memory a Absolute
constexpr uint8_t LDY_a = 0XAC; // LoaD the Y register with memory  a Absolute
constexpr uint8_t LDY_ax = 0XBC; // LoaD the Y register with memory  Absolute Indexed with X a,x
constexpr uint8_t CPY_a = 0XCC; // ComPare memory and Y register a Absolute
constexpr uint8_t CPX_a = 0XEC; // ComPare memory and X register a Absolute

constexpr uint8_t ORA_a = 0X0D; // "OR" memory with Accumulator a Absolute
constexpr uint8_t ORA_ax = 0X1D; // "OR" memory with Accumulator Absolute Indexed with X a,x
constexpr uint8_t AND_a = 0X2D; // "AND" memory with Accumulator a Absolute
constexpr uint8_t AND_ax = 0X3D; // "AND" memory with Accumulator Absolute Indexed with X a,x
constexpr uint8_t EOR_a = 0X4D; // "Exclusive OR" memory with accumulate A Absolute
constexpr uint8_t EOR_ax = 0X5D; // "Exclusive OR" memory with accumulate Absolute Indexed with X a,x
constexpr uint8_t ADC_a = 0X6D; // ADd memory to accumulator with Carry  a Absolute
constexpr uint8_t ADC_ax = 0X7D; // ADd memory to accumulator with Carry  a,x Absoulte Indexed with X
constexpr uint8_t STA_a = 0X8D; // STore Accumulator in memory a Absolute
constexpr uint8_t STA_ax = 0X9D; // STore Accumulator in memory Absolute Indexed with X a,x
constexpr uint8_t LDA_a = 0XAD; // LoaD Accumulator with memory  a Absolute
constexpr uint8_t LDA_ax = 0XBD; // LoaD Accumulator with memory Absolute Indexed with X a,x
constexpr uint8_t CMP_a = 0XCD; // CoMPare memory and accumulator a Absolute
constexpr uint8_t CMP_ax = 0XDD; // CoMPare memory and accumulator Absolute Indexed with X a,x
constexpr uint8_t SBC_a = 0XED; // SuBtract memory from accumulator with borrow (Carry bit) a Absolute
constexpr uint8_t SBC_ax = 0XFD; // SuBtract memory from accumulator with borrow (Carry bit) Absolute Indexed with X a,x

constexpr uint8_t ASL_a = 0X0E; // Arithmetic Shift one bit Left, memory or accumulator a Absolute (TODO)
constexpr uint8_t ASL_ax = 0X1E; // Arithmetic Shift one bit Left, memory or accumulator Absolute Indexed with X a,x (TODO)
constexpr uint8_t ROL_a = 0X2E; // ROtate one bit Left memory or accumulator a Absolute
constexpr uint8_t ROL_ax = 0X3E; // ROtate one bit Left memory or accumulator Absolute Indexed with X a,x
constexpr uint8_t LSR_a = 0X4E; // Logical Shift one bit Right memory or accumulator a Absolute
constexpr uint8_t LSR_ax = 0X5E; // Logical Shift one bit Right memory or accumulator Absolute Indexed with X a,x
constexpr uint8_t ROR_a = 0X6E; // ROtate one bit Right memory or accumulator a Absolute
constexpr uint8_t ROR_ax = 0X7E; // ROtate one bit Right memory or accumulator Absolute Indexed with X a,x
constexpr uint8_t STX_a = 0X8E; // STore the X register in memory a Absolute
constexpr uint8_t STZ_ax = 0X9E; // STore Zero register in memory Absolute Indexed with X a,x
constexpr uint8_t LDX_a = 0XAE; // LoaD the X register with memory a Absolute 
constexpr uint8_t LDX_ay = 0XBE; // LoaD the X register with memory Absolute Indexed with Y a,y
constexpr uint8_t DEC_a = 0XCE; // DECrement memory or accumulate by one a Absolute
constexpr uint8_t DEC_ax = 0XDE; // DECrement memory or accumulate by one Absolute Indexed with X a,x
constexpr uint8_t INC_a = 0XEE; // INCrement memory or accumulate by one a Absolute
constexpr uint8_t INC_ax = 0XFE; // INCrement memory or accumulate by one Absolute Indexed with X a,x

constexpr uint8_t BBR0 = 0X0F; // Branch on Bit Reset 0
constexpr uint8_t BBR1 = 0X1F; // Branch on Bit Reset 1
constexpr uint8_t BBR2 = 0X2F; // Branch on Bit Reset 2
constexpr uint8_t BBR3 = 0X3F; // Branch on Bit Reset 3
constexpr uint8_t BBR4 = 0X4F; // Branch on Bit Reset 4
constexpr uint8_t BBR5 = 0X5F; // Branch on Bit Reset 5
constexpr uint8_t BBR6 = 0X6F; // Branch on Bit Reset 6
constexpr uint8_t BBR7 = 0X7F; // Branch on Bit Reset 7
constexpr uint8_t BBS0 = 0X8F; // Branch of Bit Set 0
constexpr uint8_t BBS1 = 0X9F; // Branch of Bit Set 1
constexpr uint8_t BBS2 = 0XAF; // Branch of Bit Set 2
constexpr uint8_t BBS3 = 0XBF; // Branch of Bit Set 3
constexpr uint8_t BBS4 = 0XCF; // Branch of Bit Set 4
constexpr uint8_t BBS5 = 0XDF; // Branch of Bit Set 5
constexpr uint8_t BBS6 = 0XEF; // Branch of Bit Set 6
constexpr uint8_t BBS7 = 0XFF; // Branch of Bit Set 7

