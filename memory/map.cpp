#include <memory/map.hpp>

namespace map {
	Range::Range(uint32_t base, uint32_t size) : mBase(base), mSize(size), mEnd(base + size)
	{
	}

	int32_t Range::contains(uint32_t addr)
	{
		return (addr >= mBase) && (addr < mEnd) ? addr - mBase : -1;
	}

	Range::~Range()
	{
	}

	Map::Map() :
		mBIOS(0xbfc00000, 512 * 1024),
		// Memory latency and expansion mapping
		mMEM_CONTROL(0x1f801000, 36),
		// Register that has something to do with RAM configuration,
		// configured by the BIOS
		mRAM_SIZE(0x1f801060, 4)
	{
	}

	Map::~Map()
	{
	}
}