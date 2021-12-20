#include <bus.hpp>
#include "helpers.hpp"

namespace bus {
	Bus::Bus()
	{
		std::string path("roms/SCPH1001.BIN");
		mBios.loadFromFile(path);
		mCpu.connectBus(this);
		mBios.connectBus(this);
	}
	
	// Fetch the 32 bit little endian word at ‘offset‘
	uint32_t Bus::load32(uint32_t addr)
	{
		int32_t offset = mMap.mBIOS.contains(addr);
		if (offset != -1)
			return mBios.load32(offset);

		panic(fmt::format("Unhandled read at {:x}", addr));
	}

	Bus::~Bus()
	{
	}
}