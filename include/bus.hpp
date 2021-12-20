#pragma once

#include <memory/bios.hpp>
#include <memory/map.hpp>
#include <cpu/cpu.hpp>

namespace bus
{
	class Bus
	{
	public:
		Bus();
		// Fetch the 32 bit little endian word at ‘offset‘
		uint32_t load32(uint32_t addr);
		~Bus();
		map::Map mMap;
		bios::Bios mBios;
		cpu::Cpu mCpu;
	};
}