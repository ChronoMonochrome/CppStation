#pragma once

#include <cstdint>
#include "helpers.hpp"
namespace bus {
	class Bus;
}

namespace cpu {
	class Instruction {
	public:
		Instruction(uint32_t data);
		// Return bits [31:26] of the instruction
		uint32_t function();
		// Return register index in bits [20:16]
		uint32_t t();
		// Return register index in bits [25:21]
		uint32_t s();
		// Return immediate value in bits [16:0]
		uint32_t imm();
		~Instruction();
		uint32_t mData;
	};

	class Cpu {
	public:
		Cpu();

		void decodeAndExecute(Instruction &instruction);
		void runNextInstruction();
		uint32_t reg(uint32_t index);
		void setReg(uint32_t index, uint32_t val);
		uint32_t load32(uint32_t addr);
		uint32_t store32(uint32_t addr, uint32_t val);

		// Load Upper Immediate
		void opLui(Instruction &instruction);

		// Bitwise Or Immediate
		void opOri(Instruction &instruction);

		// Store Word
		void opSw(Instruction &instruction);

		~Cpu();
		// The program counter register
		uint32_t mPc;
		// General Purpose Registers.
		// The first entry must always contain 0.
		uint32_t mRegs[32];

		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link this CPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}