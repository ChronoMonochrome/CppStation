#pragma once

#include <memory/ram.hpp>
#include <memory/bios.hpp>
#include <memory/map.hpp>
#include <cpu/cpu.hpp>

namespace bus
{
	class Bus
	{
	public:
		Bus();
		// Fetch the 32 bit little endian word at ‘addr‘
		uint32_t load32(uint32_t addr);
		// Store 32bit word `val` into `addr`
		void store32(uint32_t addr, uint32_t val);
		// Load 16bit halfword at `addr`
		uint16_t load16(uint32_t addr);
		// Store 16bit halfword `val` into `addr`
		void store16(uint32_t addr, uint16_t val);
		// Load byte at `addr`
		uint8_t load8(uint32_t addr);
		// Store byte `val` into `addr`
		void store8(uint32_t addr, uint8_t val);
		~Bus();
		map::Map mMap;
		ram::Ram mRam;
		bios::Bios mBios;
		cpu::Cpu mCpu;
	};
}