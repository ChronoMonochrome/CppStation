#include <memory/map.hpp>
#include "helpers.hpp"

namespace map {

Range::Range(uint32_t base, uint32_t size) : mBase(base), mSize(size), mEnd(base + size)
{
}

Range::~Range()
{
}

Map::Map() :
	mBIOS(0x1fc00000, bios::BIOS_SIZE),
	// Memory latency and expansion mapping
	mMEM_CONTROL(0x1f801000, 36),
	// Register that has something to do with RAM configuration,
	// configured by the BIOS
	mRAM_SIZE(0x1f801060, 4),
	// Cache control register. Full address since it's in KSEG2
	mCACHE_CONTROL(0xfffe0130, 4),
	// RAM
	mRAM(0x00000000, ram::RAM_SIZE),
	mSPU(0x1f801c00, 640),
	// Expansion region 1
	mEXPANSION_1(0x1f000000, 512 * 1024),
	// Expansion region 2
	mEXPANSION_2(0x1f802000, 66),
	// Interrupt Control registers (status and mask)
	mIRQ_CONTROL(0x1f801070, 8),
	// Timer registers
	mTIMERS(0x1f801100, 0x30),
	// Direct Memory Access registers
	mDMA(0x1f801080, 0x80),
	// GPU registers
	mGPU(0x1f801810, 8)
{
}

Map::~Map()
{
}

} // namespace map