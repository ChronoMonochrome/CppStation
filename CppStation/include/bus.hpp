#pragma once

#include <cpu/cpu.hpp>
#include <gpu/gpu.hpp>
#include <memory/bios.hpp>
#include <memory/dma.hpp>
#include <memory/map.hpp>
#include <memory/ram.hpp>

namespace bus {

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

	// DMA register read
	uint32_t dmaReg(uint32_t offset);

	// DMA register write
	void setDmaReg(uint32_t offset, uint32_t val);

	// Execute DMA transfer for a port
	void doDma(dma::Port port);

	// Emulate DMA transfer for linked list synchronization mode.
	void doDmaLinkedList(dma::Port port);

	// Emulate DMA transfer for Manual and Request synchronization modes.
	void doDmaBlock(dma::Port port);

	~Bus();
	map::Map mMap;
	ram::Ram mRam;
	bios::Bios mBios;
	cpu::Cpu mCpu;
	dma::Dma mDma;
	gpu::Gpu mGpu;
};

} // namespace bus