#pragma once

#include <memory/bios.hpp>
#include <memory/ram.hpp>
#include <cstdint>

namespace map {

// Mask array used to strip the region bits of the address. The
// mask is selected using the 3 MSBs of the address so each entry
// effectively matches 512kB of the address space. KSEG2 is not
// touched since it doesn't share anything with the other
// regions.
constexpr uint32_t REGION_MASK[] = {
	0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, // KUSEG: 2048MB
	0x7fffffff,                                     // KSEG0:  512MB
	0x1fffffff,                                     // KSEG1:  512MB
	0xffffffff, 0xffffffff,	                        // KSEG2: 1024MB
};

class Range {
public:
	Range(uint32_t base, uint32_t size);
	~Range();
	uint32_t mBase, mSize, mEnd;
};

static inline constexpr int32_t contains(uint32_t addr, uint32_t end, uint32_t base)
{
	return (addr >= base) && (addr < end) ? addr - base : -1;
}

class Map {
public:
	Map();
	~Map();

	Range mBIOS;
	Range mMEM_CONTROL;
	Range mRAM_SIZE;
	Range mCACHE_CONTROL;
	Range mRAM;
	Range mSPU;
	Range mEXPANSION_1;
	Range mEXPANSION_2;
	Range mIRQ_CONTROL;
	Range mTIMERS;
	Range mDMA;
	Range mGPU;
};

// Mask a CPU address to remove the region bits.
inline constexpr uint32_t maskRegion(uint32_t addr)
{
	// Index address space in 512MB chunks
	return addr & REGION_MASK[(addr >> 29)];
}

} // namespace map