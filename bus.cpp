#include <bus.hpp>
#include "helpers.hpp"

namespace bus {
	Bus::Bus()
	{
		std::string path("roms/SCPH1001.BIN");
		uint64_t res = mBios.loadFromFile(path).check();
		(void)res;

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

	void Bus::store32(uint32_t addr, uint32_t val)
	{
		if ((addr % 4) != 0)
			panic(fmt::format("Unaligned store32 address: {:08x}", addr));

		int32_t offset = mMap.mMEM_CONTROL.contains(addr);
		if (offset != -1) {
			switch (offset)
			{
			case 0: // Expansion 1 base address
				if (val != 0x1f000000)
					panic(fmt::format("Bad expansion 1 base address: 0x{:08x}", val));
				break;
			case 4: // Expansion 2 base address
				if (val != 0x1f802000)
					panic(fmt::format("Bad expansion 2 base address: 0x{:08x}", val));
				break;
			default:
				println("Unhandled write to MEM_CONTROL register");
			}
			return;
		}

		offset = mMap.mRAM_SIZE.contains(addr);
		if (offset != -1) {
			println("Unhandled write to RAM_SIZE register");
			return;
		}

		panic(fmt::format("unhandled store32 into address {:08x}", addr));
	}

	Bus::~Bus()
	{
	}
}