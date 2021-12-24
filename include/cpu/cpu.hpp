#pragma once

#include <cstdint>
#include "helpers.hpp"
namespace bus {
	class Bus;
}

namespace cpu {
	typedef struct {
		uint32_t val;
	} registerIndex;

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
		registerIndex t();

		// Return register index in bits [25:21]
		registerIndex s();

		// Return register index in bits [15:11]
		registerIndex d();

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
		// Retrieve the value of a general purpose register
		uint32_t reg(registerIndex index);
		// Set the value of a general purpose register
		void setReg(registerIndex index, uint32_t val);
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

		~Cpu();
		// The program counter register
		uint32_t mPc;
		// Next instruction to be executed, used to simulate the branch
		// delay slot
		Instruction mNextInstruction;
		// General Purpose Registers.
		// The first entry must always contain 0.
		uint32_t mRegs[32];
		// 2nd set of registers used to emulate the load delay slot
		// accurately. They contain the output of the current
		// instruction.
		uint32_t mOutRegs[32];
		// Load initiated by the current instruction
		registerIndex mLoadRegIdx;
		uint32_t mLoadReg;

		// Cop0 register 12: Status Register
		uint32_t mSr;

		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link this CPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}