/**
 * @file InstructionTest.hh
 *
 * A test suite for Instruction.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef INSTRUCTION_TEST_HH
#define INSTRUCTION_TEST_HH

#include <TestSuite.h>
#include "Instruction.hh"
#include "TerminalRegister.hh"
#include "Move.hh"
#include "Immediate.hh"
#include "Exception.hh"
#include "TerminalImmediate.hh"
#include "RFPort.hh"
#include "Bus.hh"
#include "POMDisassembler.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class InstructionTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testInstruction();

private:
};


/**
 * Called before each test.
 */
void
InstructionTest::setUp() {
}


/**
 * Called after each test.
 */
void
InstructionTest::tearDown() {
}

/**
 * Tests that Instruction works properly.
 */
void
InstructionTest::testInstruction() {

    Instruction ins;

    TS_ASSERT_EQUALS(ins.moveCount(), 0);
    TS_ASSERT_EQUALS(ins.immediateCount(), 0);
    TS_ASSERT_THROWS(ins.move(0), OutOfRange);
    TS_ASSERT_THROWS(ins.immediate(0), OutOfRange);

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    RFPort* port1 = new RFPort("P1", *reg_file);
    RFPort* port2 = new RFPort("P2", *reg_file);
    UIntWord index1 = 10;
    UIntWord index2 = 11;

    TerminalRegister* src_reg = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg = new TerminalRegister(
        *port2, index2);

    TerminalRegister* src_reg2 = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg2 = new TerminalRegister(
        *port2, index2);

    TerminalRegister* src_reg3 = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg3 = new TerminalRegister(
        *port2, index2);

    Bus moveBus("Bus1", 32, 32, Machine::ZERO);

    Move* move1 = new Move(src_reg, dst_reg, moveBus);
    Move* move2 = new Move(src_reg2, dst_reg2, moveBus);

    TS_ASSERT_THROWS_NOTHING(ins.addMove(move1));
    TS_ASSERT_THROWS(ins.addMove(move1), ObjectAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(ins.addMove(move2));
    TS_ASSERT_EQUALS(ins.moveCount(), 2);

    TS_ASSERT_THROWS(ins.move(ins.moveCount()), OutOfRange);
    TS_ASSERT_THROWS(ins.move(-1), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(ins.move(ins.moveCount()-1));
    TS_ASSERT_EQUALS(&ins.move(0), move1);

    Machine dummy_mach;
    InstructionTemplate ins_template("dummy", dummy_mach);
    SimValue imm_value = SimValue(1232, 32);

    Immediate* imm1 = new Immediate(
        new TerminalImmediate(imm_value), src_reg3);

    Immediate* imm2 = new Immediate(
        new TerminalImmediate(imm_value), dst_reg3);

    TS_ASSERT_THROWS_NOTHING(ins.addImmediate(imm1));
    TS_ASSERT_THROWS(ins.addImmediate(imm1), ObjectAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(ins.addImmediate(imm2));
    TS_ASSERT_EQUALS(ins.immediateCount(), 2);

    TS_ASSERT_THROWS(ins.immediate(ins.immediateCount()), OutOfRange);
    TS_ASSERT_THROWS(ins.immediate(-1), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(ins.immediate(ins.immediateCount()-1));
    TS_ASSERT_EQUALS(&ins.immediate(0), imm1);

    TS_ASSERT_EQUALS(
        POMDisassembler::disassemble(ins), 
        POMDisassembler::disassemble(*(ins.copy())));

    delete reg_file;
}

#endif
