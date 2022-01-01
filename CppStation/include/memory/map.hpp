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
	const uint32_t REGION_MASK[] = {
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, // KUSEG: 2048MB
		0x7fffffff,                                     // KSEG0:  512MB
		0x1fffffff,                                     // KSEG1:  512MB
		0xffffffff, 0xffffffff,	                        // KSEG2: 1024MB
	};

	class Range {
	public:
		Range(uint32_t base, uint32_t size);
		int32_t contains(uint32_t addr);
		~Range();
	private:
		uint32_t mBase, mSize, mEnd;
	};

	class Map {
	public:
		Map();
		~Map();
		// Mask a CPU address to remove the region bits.
		uint32_t maskRegion(uint32_t addr);
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
	};
}