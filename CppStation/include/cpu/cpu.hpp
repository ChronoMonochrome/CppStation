#pragma once

#include <cstdint>
#include "helpers.hpp"
namespace bus {
class Bus;
}

namespace cpu {

struct RegisterIndex
{
	uint32_t val;
};

namespace exception {
	enum Exception
	{
		// Address error on load
		LoadAddressError = 0x4,
		// Address error on store
		StoreAddressError = 0x5,
		// System call (caused by the SYSCALL opcode)
		SysCall = 0x8,
		// Breakpoint (caused by the BREAK opcode)
		Break = 0x9,
		// CPU encountered an unknown instruction
		IllegalInstruction = 0xa,
		// Unsupported coprocessor operation
		CoprocessorError = 0xb,
		// Arithmetic overflow
		Overflow = 0xc,
	};
}

namespace Instruction 
{

// Return bits [31:26] of the instruction
static inline constexpr uint32_t function(uint32_t instruction)
{
	return instruction >> 26;
}

// Return bits [5:0] of the instruction
static inline constexpr uint32_t subfunction(uint32_t instruction)
{
	return instruction & 0x3f;
}

// Return coprocessor opcode in bits [25:21]
static inline constexpr uint32_t copOpcode(uint32_t instruction)
{
	return (instruction >> 21) & 0x1f;
}

// Return register index in bits [20:16]
static inline constexpr RegisterIndex t(uint32_t instruction)
{
	return {(instruction >> 16) & 0x1f};
}

// Return register index in bits [25:21]
static inline constexpr RegisterIndex s(uint32_t instruction)
{
	return {(instruction >> 21) & 0x1f};
}

// Return register index in bits [15:11]
static inline constexpr RegisterIndex d(uint32_t instruction)
{
	return {(instruction >> 11) & 0x1f};
}

// Return immediate value in bits [16:0]
static inline constexpr uint32_t imm(uint32_t instruction)
{
	return instruction & 0xffff;
}

// Return immediate value in bits [16:0] as a sign-extended 32bit value
static inline constexpr uint32_t imm_se(uint32_t instruction)
{
	return (uint32_t)((int16_t)(instruction & 0xffff));
}

// Jump target stored in bits [25:0]
static inline constexpr uint32_t imm_jump(uint32_t instruction)
{
	return instruction & 0x3ffffff;
}

// Shift Immediate values are stored in bits [10:6]
static inline constexpr uint32_t shift(uint32_t instruction)
{
	return (instruction >> 6) & 0x1f;
}

} // namespace Instruction

class Cpu {
public:
	Cpu();
	~Cpu();

	// Decode `instruction`'s opcode and run the function
	void decodeAndExecute(uint32_t instruction);
	void runNextInstruction();
	// Trigger an exception
	void exception(enum exception::Exception cause);

	// System Call
	void opSyscall(uint32_t instruction);
	// Retrieve the value of a general purpose register
	uint32_t reg(RegisterIndex index);
	// Set the value of a general purpose register
	void setReg(RegisterIndex index, uint32_t val);
	// Load 32bit value from the bus
	uint32_t load32(uint32_t addr);
	// Store 32bit value into the memory
	void store32(uint32_t addr, uint32_t val);
	// Load 16bit value from the memory
	uint16_t load16(uint32_t addr);
	// Store 16bit value into the memory
	void store16(uint32_t addr, uint16_t val);
	// Load 8bit value from the memory
	uint8_t load8(uint32_t addr);
	// Store 8bit value into the memory
	void store8(uint32_t addr, uint8_t val);

	// Load Upper Immediate
	void opLui(uint32_t instruction);

	// Bitwise Or Immediate
	void opOri(uint32_t instruction);

	// Store Word
	void opSw(uint32_t instruction);

	// Shift Left Logical
	void opSll(uint32_t instruction);

	// Add Immediate Unsigned
	void opAddiu(uint32_t instruction);

	// Jump
	void opJ(uint32_t instruction);

	// Bitwise Or
	void opOr(uint32_t instruction);

	// Coprocessor 0 opcode
	void opCop0(uint32_t instruction);

	// Move To Coprocessor 0
	void opMtc0(uint32_t instruction);

	// Branch to immediate value `offset`
	void branch(uint32_t offset);

	// Branch if Not Equal
	void opBne(uint32_t instruction);

	// Add Immediate and check for signed overflow
	void opAddi(uint32_t instruction);

	// Load Word
	void opLw(uint32_t instruction);

	// Set on Less Than Unsigned
	void opSltu(uint32_t instruction);

	// Add Unsigned
	void opAddu(uint32_t instruction);

	// Store Halfword
	void opSh(uint32_t instruction);

	// Jump And Link
	void opJal(uint32_t instruction);

	// Bitwise And Immediate
	void opAndi(uint32_t instruction);

	// Store Byte
	void opSb(uint32_t instruction);

	// Jump Register
	void opJr(uint32_t instruction);

	// Load Byte (signed)
	void opLb(uint32_t instruction);

	// Branch if Equal
	void opBeq(uint32_t instruction);

	// Move From Coprocessor 0
	void opMfc0(uint32_t instruction);

	// Bitwise And
	void opAnd(uint32_t instruction);

	// Add and check for signed overflow
	void opAdd(uint32_t instruction);

	// Branch if Greater Than Zero
	void opBgtz(uint32_t instruction);

	// Branch if Less than or Equal to Zero
	void opBlez(uint32_t instruction);

	// Load Byte Unsigned
	void opLbu(uint32_t instruction);

	// Jump And Link Register
	void opJalr(uint32_t instruction);

	// Various branch instructions: BGEZ, BLTZ, BGEZAL, BLTZAL.
	// Bits 16 and 20 are used to figure out which one to use.
	void opBxx(uint32_t instruction);

	// Set if Less Than Immediate (signed)
	void opSlti(uint32_t instruction);

	// Substract Unsigned
	void opSubu(uint32_t instruction);

	// Shift Right Arithmetic
	void opSra(uint32_t instruction);

	// Divide (signed)
	void opDiv(uint32_t instruction);

	// Move From LO
	void opMflo(uint32_t instruction);

	// Shift Right Logical
	void opSrl(uint32_t instruction);

	// Set if Less Than Immediate Unsigned
	void opSltiu(uint32_t instruction);

	// Divide Unsigned
	void opDivu(uint32_t instruction);

	// Move From HI
	void opMfhi(uint32_t instruction);

	// Set on Less Than (signed)
	void opSlt(uint32_t instruction);

	// Move to LO
	void opMtlo(uint32_t instruction);

	// Move to HI
	void opMthi(uint32_t instruction);

	// Return From Exception
	void opRfe(uint32_t instruction);

	// Load Halfword Unsigned
	void opLhu(uint32_t instruction);

	// Shift Left Logical Variable
	void opSllv(uint32_t instruction);

	// Load Halfword (signed)
	void opLh(uint32_t instruction);

	// Bitwise Not Or
	void opNor(uint32_t instruction);

	// Shift Right Arithmetic Variable
	void opSrav(uint32_t instruction);

	// Shift Right Logical Variable
	void opSrlv(uint32_t instruction);

	// Multiply Unsigned
	void opMultu(uint32_t instruction);

	// Bitwise Exclusive Or
	void opXor(uint32_t instruction);

	// Break
	void opBreak(uint32_t instruction);

	// Multiply (signed)
	void opMult(uint32_t instruction);

	// Substract and check for signed overflow
	void opSub(uint32_t instruction);

	// Bitwise eXclusive Or Immediate
	void opXori(uint32_t instruction);

	// Coprocessor 1 opcode (does not exist on the Playstation)
	void opCop1(uint32_t instruction);
	// Coprocessor 2 opcode (GTE)
	void opCop2(uint32_t instruction);
	// Coprocessor 3 opcode (does not exist on the Playstation)
	void opCop3(uint32_t instruction);

	// Load Word Left (little-endian only implementation)
	void opLwl(uint32_t instruction);

	// Load Word Right (little-endian only implementation)
	void opLwr(uint32_t instruction);

	// Store Word Left (little-endian only implementation)
	void opSwl(uint32_t instruction);

	// Store Word Right (little-endian only implementation)
	void opSwr(uint32_t instruction);

	// Load Word in Coprocessor 0
	void opLwc0(uint32_t instruction);
	// Load Word in Coprocessor 1
	void opLwc1(uint32_t instruction);
	// Load Word in Coprocessor 2
	void opLwc2(uint32_t instruction);
	// Load Word in Coprocessor 3
	void opLwc3(uint32_t instruction);
	// Store Word in Coprocessor 0
	void opSwc0(uint32_t instruction);
	// Store Word in Coprocessor 1
	void opSwc1(uint32_t instruction);
	// Store Word in Coprocessor 2
	void opSwc2(uint32_t instruction);
	// Store Word in Coprocessor 3
	void opSwc3(uint32_t instruction);

	// Illegal instruction
	void opIllegal(uint32_t instruction);

	// The program counter register
	uint32_t mPc;
	// Instruction count
	uint32_t mIp;
	// Next value for the PC, used to simulate the branch delay slot
	uint32_t mNextPc;
	// Address of the instruction currently being executed. Used for setting the EPC in exceptions.
	uint32_t mCurrentPc;
	// General Purpose Registers.
	// The first entry must always contain 0.
	uint32_t mRegs[32];
	// 2nd set of registers used to emulate the load delay slot
	// accurately. They contain the output of the current
	// instruction.
	uint32_t mOutRegs[32];
	// Load initiated by the current instruction
	RegisterIndex mLoadRegIdx;
	uint32_t mLoadReg;

	// Cop0 register 12: Status Register
	uint32_t mSr;
	// Cop0 register 13: Cause Register
	uint32_t mCause;
	// Cop0 register 14: EPC
	uint32_t mEpc;

	// HI register for division remainder and multiplication high result
	uint32_t mHi;
	// LO register for division quotient and multiplication low result
	uint32_t mLo;

	// Set by the current instruction if a branch occured and the
	// next instruction will be in the delay slot.
	bool mBranch;
	// Set if the current instruction executes in the delay slot
	bool mDelaySlot;

	// Linkage to the communications bus
	bus::Bus *mBus = nullptr;
	// Link this CPU to a communications bus
	void connectBus(bus::Bus *n) { mBus = n; }
};

} // namespace cpu