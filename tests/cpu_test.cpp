#include "w65c02s/cpu.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <initializer_list>

namespace w65c02s {
namespace {

constexpr std::uint16_t origin = 0x0200;

class CpuTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        memory.write_word(vector::reset, origin);
        cpu.reset();
    }

    void program(std::initializer_list<std::uint8_t> bytes) { memory.load({bytes.begin(), bytes.size()}, origin); }

    void run(int instructions)
    {
        for (int i = 0; i < instructions; ++i) {
            cpu.step();
        }
    }

    [[nodiscard]] bool flag_set(std::uint8_t mask) const { return (cpu.regs().p & mask) != 0; }

    Memory memory;
    Cpu cpu{memory};
};

TEST_F(CpuTest, ResetLoadsVectorAndInitialState)
{
    EXPECT_EQ(cpu.regs().pc, origin);
    EXPECT_EQ(cpu.regs().s, 0xFD);
    EXPECT_TRUE(flag_set(flag::I));
    EXPECT_FALSE(flag_set(flag::D));
}

TEST_F(CpuTest, LoadImmediateSetsZeroAndNegative)
{
    program({0xA9, 0x00, 0xA9, 0x80}); // LDA #$00; LDA #$80
    run(1);
    EXPECT_TRUE(flag_set(flag::Z));
    EXPECT_FALSE(flag_set(flag::N));
    run(1);
    EXPECT_EQ(cpu.regs().a, 0x80);
    EXPECT_FALSE(flag_set(flag::Z));
    EXPECT_TRUE(flag_set(flag::N));
}

TEST_F(CpuTest, AdcSetsOverflowOnSignedOverflow)
{
    program({0x18, 0xA9, 0x50, 0x69, 0x50}); // CLC; LDA #$50; ADC #$50
    run(3);
    EXPECT_EQ(cpu.regs().a, 0xA0);
    EXPECT_TRUE(flag_set(flag::V));
    EXPECT_FALSE(flag_set(flag::C));
    EXPECT_TRUE(flag_set(flag::N));
}

TEST_F(CpuTest, AdcSetsCarryWithoutOverflow)
{
    program({0x18, 0xA9, 0xFF, 0x69, 0x01}); // CLC; LDA #$FF; ADC #$01
    run(3);
    EXPECT_EQ(cpu.regs().a, 0x00);
    EXPECT_TRUE(flag_set(flag::C));
    EXPECT_FALSE(flag_set(flag::V));
    EXPECT_TRUE(flag_set(flag::Z));
}

TEST_F(CpuTest, SbcUsesCarryAsInvertedBorrow)
{
    program({0x38, 0xA9, 0x05, 0xE9, 0x06}); // SEC; LDA #$05; SBC #$06
    run(3);
    EXPECT_EQ(cpu.regs().a, 0xFF);
    EXPECT_FALSE(flag_set(flag::C));
    EXPECT_TRUE(flag_set(flag::N));
}

TEST_F(CpuTest, SbcSetsOverflowOnSignedOverflow)
{
    program({0x38, 0xA9, 0x80, 0xE9, 0x01}); // SEC; LDA #$80; SBC #$01  (-128 - 1)
    run(3);
    EXPECT_EQ(cpu.regs().a, 0x7F);
    EXPECT_TRUE(flag_set(flag::V));
    EXPECT_TRUE(flag_set(flag::C));
}

TEST_F(CpuTest, DecimalModeAddsAndSubtractsBcd)
{
    program({0xF8, 0x18, 0xA9, 0x58, 0x69, 0x46, // SED; CLC; LDA #$58; ADC #$46 -> $04, C=1
             0x38, 0xA9, 0x10, 0xE9, 0x01});     // SEC; LDA #$10; SBC #$01 -> $09
    run(4);
    EXPECT_EQ(cpu.regs().a, 0x04);
    EXPECT_TRUE(flag_set(flag::C));
    run(3);
    EXPECT_EQ(cpu.regs().a, 0x09);
    EXPECT_TRUE(flag_set(flag::C));
}

TEST_F(CpuTest, CompareSetsCarryWhenRegisterIsGreaterOrEqual)
{
    program({0xA2, 0x10, 0xE0, 0x10, 0xE0, 0x11}); // LDX #$10; CPX #$10; CPX #$11
    run(2);
    EXPECT_TRUE(flag_set(flag::C));
    EXPECT_TRUE(flag_set(flag::Z));
    run(1);
    EXPECT_FALSE(flag_set(flag::C));
    EXPECT_TRUE(flag_set(flag::N));
}

TEST_F(CpuTest, ZeroPageIndexedWrapsWithinZeroPage)
{
    memory.write(0x007F, 0x42);
    program({0xA2, 0xFF, 0xB5, 0x80}); // LDX #$FF; LDA $80,X -> $7F, not $017F
    run(2);
    EXPECT_EQ(cpu.regs().a, 0x42);
}

TEST_F(CpuTest, IndirectIndexedReadsSixteenBitPointer)
{
    memory.write_word(0x0010, 0x1230);
    memory.write(0x1234, 0x99);
    program({0xA0, 0x04, 0xB1, 0x10}); // LDY #$04; LDA ($10),Y
    run(2);
    EXPECT_EQ(cpu.regs().a, 0x99);
}

TEST_F(CpuTest, ZeroPageIndirectPointerWrapsAtPageEnd)
{
    memory.write(0x00FF, 0x34);
    memory.write(0x0000, 0x12);
    memory.write(0x1234, 0x77);
    program({0xB2, 0xFF}); // LDA ($FF)
    run(1);
    EXPECT_EQ(cpu.regs().a, 0x77);
}

TEST_F(CpuTest, StackLivesInPageOne)
{
    program({0xA9, 0xAB, 0x48, 0xA9, 0x00, 0x68}); // LDA #$AB; PHA; LDA #$00; PLA
    run(2);
    EXPECT_EQ(memory.read(0x01FD), 0xAB);
    EXPECT_EQ(cpu.regs().s, 0xFC);
    run(2);
    EXPECT_EQ(cpu.regs().a, 0xAB);
    EXPECT_EQ(cpu.regs().s, 0xFD);
    EXPECT_TRUE(flag_set(flag::N));
}

TEST_F(CpuTest, JsrAndRtsRoundTrip)
{
    program({0x20, 0x10, 0x02, 0xDB}); // JSR $0210; STP
    memory.write(0x0210, 0x60);        // RTS
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0210);
    EXPECT_EQ(memory.read_word(0x01FC), 0x0202); // return address - 1
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0203);
    run(1);
    EXPECT_TRUE(cpu.stopped());
}

TEST_F(CpuTest, BrkJumpsThroughIrqVectorAndRtiReturns)
{
    memory.write_word(vector::irq, 0x0300);
    memory.write(0x0300, 0x40);        // RTI
    program({0xF8, 0x00, 0xEA, 0xEA}); // SED; BRK; signature byte; NOP
    run(2);
    EXPECT_EQ(cpu.regs().pc, 0x0300);
    EXPECT_TRUE(flag_set(flag::I));
    EXPECT_FALSE(flag_set(flag::D));
    EXPECT_TRUE((memory.read(0x01FB) & flag::B) != 0);
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0203);
    EXPECT_TRUE(flag_set(flag::D));
}

TEST_F(CpuTest, IrqIsMaskedByInterruptDisableFlag)
{
    memory.write_word(vector::irq, 0x0300);
    program({0xEA, 0x58}); // NOP; CLI
    run(1);
    cpu.irq();
    EXPECT_EQ(cpu.regs().pc, 0x0201);
    run(1);
    cpu.irq();
    EXPECT_EQ(cpu.regs().pc, 0x0300);
    EXPECT_FALSE((memory.read(0x01FB) & flag::B) != 0);
}

TEST_F(CpuTest, NmiIgnoresInterruptDisableFlag)
{
    memory.write_word(vector::nmi, 0x0400);
    program({0xEA});
    cpu.nmi();
    EXPECT_EQ(cpu.regs().pc, 0x0400);
}

TEST_F(CpuTest, WaiSuspendsUntilInterrupt)
{
    program({0xCB, 0xE8}); // WAI; INX
    run(1);
    EXPECT_TRUE(cpu.waiting());
    run(5);
    EXPECT_EQ(cpu.regs().x, 0x00);
    cpu.irq(); // masked: resumes after WAI without servicing
    EXPECT_FALSE(cpu.waiting());
    run(1);
    EXPECT_EQ(cpu.regs().x, 0x01);
}

TEST_F(CpuTest, BranchOffsetIsRelativeToNextInstruction)
{
    program({0x80, 0x02, 0xEA, 0xEA, 0x80, 0xFA}); // BRA +2; NOP; NOP; BRA -6
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0204);
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0200);
}

TEST_F(CpuTest, BbrAndBbsTestBitInZeroPageMemory)
{
    memory.write(0x0010, 0b0000'0100);
    program({0x2F, 0x10, 0x10,   // BBR2 $10,+16 -> not taken
             0xAF, 0x10, 0x10}); // BBS2 $10,+16 -> taken
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0203);
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0216);
}

TEST_F(CpuTest, RmbAndSmbChangeSingleBit)
{
    memory.write(0x0020, 0xFF);
    program({0x37, 0x20, 0xC7, 0x21}); // RMB3 $20; SMB4 $21
    run(2);
    EXPECT_EQ(memory.read(0x0020), 0xF7);
    EXPECT_EQ(memory.read(0x0021), 0x10);
}

TEST_F(CpuTest, RotatesShiftThroughCarry)
{
    program({0x38, 0xA9, 0x80, 0x2A, 0x6A, 0x6A}); // SEC; LDA #$80; ROL A; ROR A; ROR A
    run(3);
    EXPECT_EQ(cpu.regs().a, 0x01);
    EXPECT_TRUE(flag_set(flag::C));
    run(1);
    EXPECT_EQ(cpu.regs().a, 0x80);
    EXPECT_TRUE(flag_set(flag::C));
    run(1);
    EXPECT_EQ(cpu.regs().a, 0xC0);
    EXPECT_FALSE(flag_set(flag::C));
}

TEST_F(CpuTest, TsbAndTrbSetZeroFromOriginalValue)
{
    memory.write(0x0030, 0x0F);
    program({0xA9, 0xF0, 0x04, 0x30, 0x14, 0x30}); // LDA #$F0; TSB $30; TRB $30
    run(2);
    EXPECT_EQ(memory.read(0x0030), 0xFF);
    EXPECT_TRUE(flag_set(flag::Z));
    run(1);
    EXPECT_EQ(memory.read(0x0030), 0x0F);
    EXPECT_FALSE(flag_set(flag::Z));
}

TEST_F(CpuTest, IncrementAndTransferSetFlags)
{
    program({0xA2, 0xFF, 0xE8, 0x8A}); // LDX #$FF; INX; TXA
    run(2);
    EXPECT_EQ(cpu.regs().x, 0x00);
    EXPECT_TRUE(flag_set(flag::Z));
    run(1);
    EXPECT_TRUE(flag_set(flag::Z));
}

TEST_F(CpuTest, ReservedOpcodesAreNopsWithDefinedLength)
{
    program({0x02, 0xFF, 0x5C, 0xFF, 0xFF, 0x03});
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0202);
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0205);
    run(1);
    EXPECT_EQ(cpu.regs().pc, 0x0206);
}

TEST_F(CpuTest, StpHaltsUntilReset)
{
    program({0xDB, 0xE8}); // STP; INX
    run(3);
    EXPECT_TRUE(cpu.stopped());
    EXPECT_EQ(cpu.regs().x, 0x00);
    cpu.reset();
    EXPECT_FALSE(cpu.stopped());
}

} // namespace
} // namespace w65c02s