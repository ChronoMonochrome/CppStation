#include <bus.hpp>
#include "helpers.hpp"

namespace bus {
	Bus::Bus()
	{
		std::string path("roms/SCPH1001.BIN");
		uint64_t res = mBios.loadFromFile(path).check();
		(void)res;

		mRam.connectBus(this);
		mCpu.connectBus(this);
		mBios.connectBus(this);
	}

	uint32_t Bus::load32(uint32_t addr)
	{
		if ((addr % 4) != 0)
			panic(fmt::format("Unaligned load32 address: {:08x}", addr));

		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1)
			return mRam.load32(offset);

		offset = mMap.mBIOS.contains(abs_addr);
		if (offset != -1)
			return mBios.load32(offset);

		panic(fmt::format("{}: Unhandled read at {:08x}", __func__, abs_addr));
	}

	void Bus::store32(uint32_t addr, uint32_t val)
	{
		if ((addr % 4) != 0)
			panic(fmt::format("Unaligned store32 address: {:08x}", addr));

		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1) {
			mRam.store32(offset, val);
			return;
		}

		offset = mMap.mMEM_CONTROL.contains(abs_addr);
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

		offset = mMap.mRAM_SIZE.contains(abs_addr);
		if (offset != -1) {
			println("Unhandled write to RAM_SIZE register");
			return;
		}

		offset = mMap.mCACHE_CONTROL.contains(abs_addr);
		if (offset != -1) {
			println("Unhandled write to CACHE_CONTROL register");
			return;
		}

		panic(fmt::format("unhandled store32 into address {:08x}", abs_addr));
	}

	uint16_t Bus::load16(uint32_t addr)
	{
		panic(fmt::format("unhandled load16 at address {:08x}", addr));
	}

	void Bus::store16(uint32_t addr, uint16_t val)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mSPU.contains(abs_addr);
		if (offset != -1)
		{
			println(fmt::format("Unhandled write to SPU register {:08x}: {:04x}",
					 abs_addr, val));
			return;
		}
		panic(fmt::format("unhandled store16 into address {:08x}", addr));
	}

	uint8_t Bus::load8(uint32_t addr)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1)
		{
			return mRam.load8(offset);
		}
		offset = mMap.mBIOS.contains(abs_addr);
		if (offset != -1)
		{
			return mBios.load8(offset);
		}

		offset = mMap.mEXPANSION_1.contains(abs_addr);
		if (offset != -1)
		{
			// No expansion implemented
			return 0xff;
		}

		panic(fmt::format("unhandled load8 at address {:08x}", addr));
	}

	void Bus::store8(uint32_t addr, uint8_t val)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1)
		{
			mRam.store8(offset, val);
			return;
		}
		offset = mMap.mEXPANSION_2.contains(abs_addr);
		if (offset != -1)
		{
			println(fmt::format("Unhandled write to expansion 2 register {:08x}: {:02x}", abs_addr, val));
			return;
		}

		panic(fmt::format("unhandled store8 into address {:08x}: {:02x}", addr, val));
	}

	Bus::~Bus()
	{
	}
}