#pragma once

#include <cstdint>
#include "helpers.hpp"
namespace bus {
	class Bus;
}

namespace cpu {
	class Cpu {
	public:
		Cpu();
		uint32_t load32(uint32_t addr);
		void decodeAndExecute(uint32_t instruction);
		void runNextInstruction();

		~Cpu();
		uint32_t mPc;
		
		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link this CPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}