#include <bus.hpp>
#include "helpers.hpp"

namespace bus {
	Bus::Bus()
	{
		std::string path("roms/SCPH1001.BIN");
		uint64_t res = mBios.loadFromFile(path).check();
		mCpu.connectBus(this);
		mBios.connectBus(this);
	}

	uint32_t Bus::load32(uint32_t addr)
	{
		if ((addr % 4) != 0)
			panic(fmt::format("Unaligned load32 address: {:08x}", addr));

		int32_t offset = mMap.mBIOS.contains(addr);
		if (offset != -1)
			return mBios.load32(offset);

		panic(fmt::format("Unhandled read at {:x}", addr));
	}

	uint32_t Bus::store32(uint32_t addr, uint32_t val)
	{
		if ((addr % 4) != 0)
			panic(fmt::format("Unaligned store32 address: {:08x}", addr));

		panic(fmt::format("unhandled store32 into address {:08x}", addr));
	}

	Bus::~Bus()
	{
	}
}