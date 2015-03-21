#pragma once

#include <cstdint>
#include "helpers.hpp"
namespace bus {
	class Bus;
}

namespace cpu {
	class RegisterIndex
	{
	public:
		RegisterIndex(uint32_t _val) : val(_val)
		{
		}
		~RegisterIndex()
		{
		}
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
			// Unsupported coprocessor operation
			CoprocessorError = 0xb,
			// Arithmetic overflow
			Overflow = 0xc,
		};
	}

	class Instruction {
	public:
		Instruction(uint32_t data);
		// Return bits [31:26] of the instruction
		uint32_t function();

		// Return bits [5:0] of the instruction
		uint32_t subfunction();

		// Return coprocessor opcode in bits [25:21]
		uint32_t copOpcode();

		// Return register index in bits [20:16]
		RegisterIndex t();

		// Return register index in bits [25:21]
		RegisterIndex s();

		// Return register index in bits [15:11]
		RegisterIndex d();

		// Return immediate value in bits [16:0]
		uint32_t imm();

		// Return immediate value in bits [16:0] as a sign-extended 32bit value
		uint32_t imm_se();

		// Jump target stored in bits [25:0]
		uint32_t imm_jump();

		// Shift Immediate values are stored in bits [10:6]
		uint32_t shift();

		~Instruction();
		uint32_t mData;
	};

	class Cpu {
	public:
		Cpu();

		// Decode `instruction`'s opcode and run the function
		void decodeAndExecute(Instruction &instruction);
		void runNextInstruction();
		// Trigger an exception
		void exception(enum exception::Exception cause);

		// System Call
		void opSyscall(Instruction &instruction);
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
		void opLui(Instruction &instruction);

		// Bitwise Or Immediate
		void opOri(Instruction &instruction);

		// Store Word
		void opSw(Instruction &instruction);

		// Shift Left Logical
		void opSll(Instruction &instruction);

		// Add Immediate Unsigned
		void opAddiu(Instruction &instruction);

		// Jump
		void opJ(Instruction &instruction);

		// Bitwise Or
		void opOr(Instruction &instruction);

		// Coprocessor 0 opcode
		void opCop0(Instruction &instruction);

		// Move To Coprocessor 0
		void opMtc0(Instruction &instruction);

		// Branch to immediate value `offset`
		void branch(uint32_t offset);

		// Branch if Not Equal
		void opBne(Instruction &instruction);

		// Add Immediate and check for signed overflow
		void opAddi(Instruction &instruction);

		// Load Word
		void opLw(Instruction &instruction);

		// Set on Less Than Unsigned
		void opSltu(Instruction &instruction);

		// Add Unsigned
		void opAddu(Instruction &instruction);

		// Store Halfword
		void opSh(Instruction &instruction);

		// Jump And Link
		void opJal(Instruction &instruction);

		// Bitwise And Immediate
		void opAndi(Instruction &instruction);

		// Store Byte
		void opSb(Instruction &instruction);

		// Jump Register
		void opJr(Instruction &instruction);

		// Load Byte (signed)
		void opLb(Instruction &instruction);

		// Branch if Equal
		void opBeq(Instruction &instruction);

		// Move From Coprocessor 0
		void opMfc0(Instruction &instruction);

		// Bitwise And
		void opAnd(Instruction &instruction);

		// Add and check for signed overflow
		void opAdd(Instruction &instruction);

		// Branch if Greater Than Zero
		void opBgtz(Instruction &instruction);

		// Branch if Less than or Equal to Zero
		void opBlez(Instruction &instruction);

		// Load Byte Unsigned
		void opLbu(Instruction &instruction);

		// Jump And Link Register
		void opJalr(Instruction &instruction);

		// Various branch instructions: BGEZ, BLTZ, BGEZAL, BLTZAL.
		// Bits 16 and 20 are used to figure out which one to use.
		void opBxx(Instruction &instruction);

		// Set if Less Than Immediate (signed)
		void opSlti(Instruction &instruction);

		// Substract Unsigned
		void opSubu(Instruction &instruction);

		// Shift Right Arithmetic
		void opSra(Instruction &instruction);

		// Divide (signed)
		void opDiv(Instruction &instruction);

		// Move From LO
		void opMflo(Instruction &instruction);

		// Shift Right Logical
		void opSrl(Instruction &instruction);

		// Set if Less Than Immediate Unsigned
		void opSltiu(Instruction &instruction);

		// Divide Unsigned
		void opDivu(Instruction &instruction);

		// Move From HI
		void opMfhi(Instruction &instruction);

		// Set on Less Than (signed)
		void opSlt(Instruction &instruction);

		// Move to LO
		void opMtlo(Instruction &instruction);

		// Move to HI
		void opMthi(Instruction &instruction);

		// Return From Exception
		void opRfe(Instruction &instruction);

		// Load Halfword Unsigned
		void opLhu(Instruction &instruction);

		// Shift Left Logical Variable
		void opSllv(Instruction &instruction);

		// Load Halfword (signed)
		void opLh(Instruction &instruction);

		// Bitwise Not Or
		void opNor(Instruction &instruction);

		// Shift Right Arithmetic Variable
		void opSrav(Instruction &instruction);

		// Shift Right Logical Variable
		void opSrlv(Instruction &instruction);

		// Multiply Unsigned
		void opMultu(Instruction &instruction);

		// Bitwise Exclusive Or
		void opXor(Instruction &instruction);

		// Break
		void opBreak(Instruction &instruction);

		// Multiply (signed)
		void opMult(Instruction &instruction);

		// Substract and check for signed overflow
		void opSub(Instruction &instruction);

		// Bitwise eXclusive Or Immediate
		void opXori(Instruction &instruction);

		// Coprocessor 1 opcode (does not exist on the Playstation)
		void opCop1(Instruction &instruction);
		// Coprocessor 2 opcode (GTE)
		void opCop2(Instruction &instruction);
		// Coprocessor 3 opcode (does not exist on the Playstation)
		void opCop3(Instruction &instruction);

		// Load Word Left (little-endian only implementation)
		void opLwl(Instruction &instruction);

		// Load Word Right (little-endian only implementation)
		void opLwr(Instruction &instruction);

		// Store Word Left (little-endian only implementation)
		void opSwl(Instruction &instruction);

		// Store Word Right (little-endian only implementation)
		void opSwr(Instruction &instruction);

		~Cpu();
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
}
