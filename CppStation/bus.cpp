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
		mDma.connectBus(this);
		mGpu.connectBus(this);
	}

	Bus::~Bus()
	{
	}

	uint32_t Bus::load32(uint32_t addr)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1)
			return mRam.load32(offset);

		offset = mMap.mBIOS.contains(abs_addr);
		if (offset != -1)
			return mBios.load32(offset);

		offset = mMap.mIRQ_CONTROL.contains(abs_addr);
		if (offset != -1)
		{
			println("IRQ control read {:x}", offset);
			return 0;
		}

		offset = mMap.mDMA.contains(abs_addr);
		if (offset != -1)
		{
			uint32_t res = dmaReg(offset);
			println("DMA read: {:08x} ==> {:08x}", abs_addr, res);
			return res;
		}

		offset = mMap.mGPU.contains(abs_addr);
		if (offset != -1)
		{
			println("GPU read: {}", offset);
			switch (offset)
			{
			case 4:
				return mGpu.status();
			default:
				panic("Unhandled GPU read {}", offset);
				return 0;
			}
		}

		offset = mMap.mTIMERS.contains(abs_addr);
		if (offset != -1)
		{
			println("Unhandled read from timer register {:x}", offset);
			return 0;
		}

		panic("{}: Unhandled load32 at address {:08x}", __func__, abs_addr);
	}

	void Bus::store32(uint32_t addr, uint32_t val)
	{
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
					panic("Bad expansion 1 base address: 0x{:08x}", val);
				break;
			case 4: // Expansion 2 base address
				if (val != 0x1f802000)
					panic("Bad expansion 2 base address: 0x{:08x}", val);
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

		offset = mMap.mIRQ_CONTROL.contains(abs_addr);
		if (offset != -1) {
			println("IRQ control: {:x} <- {:08x}", offset, val);
			return;
		}

		offset = mMap.mDMA.contains(abs_addr);
		if (offset != -1) {
			println("DMA write: {:08x} {:08x}", abs_addr, val);
			setDmaReg(offset, val);
			return;
		}

		offset = mMap.mGPU.contains(abs_addr);
		if (offset != -1) {
			switch (offset)
			{
			case 0:
				mGpu.gp0(val);
				break;
			case 4:
				mGpu.gp1(val);
				break;
			default:
				panic("GPU write {}: {:08x}", offset, val);
			}
			return;
		}

		offset = mMap.mTIMERS.contains(abs_addr);
		if (offset != -1) {
			println("Unhandled write to timer register {:x}: {:08x}", offset, val);
			return;
		}

		panic("unhandled store32 into address {:08x}", abs_addr);
	}

	uint16_t Bus::load16(uint32_t addr)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);

		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1)
			return mRam.load16(offset);

		offset = mMap.mSPU.contains(abs_addr);
		if (offset != -1)
		{
			println("Unhandled read from SPU register {:08x}", abs_addr);
			return 0;
		}

		offset = mMap.mIRQ_CONTROL.contains(abs_addr);
		if (offset != -1)
		{
			println("IRQ control read {:x}", offset);
			return 0;
		}

		panic("unhandled load16 at address {:08x}", addr);
	}

	void Bus::store16(uint32_t addr, uint16_t val)
	{
		uint32_t abs_addr = mMap.maskRegion(addr);
		int32_t offset = mMap.mRAM.contains(abs_addr);
		if (offset != -1) {
			mRam.store16(offset, val);
			return;
		}

		offset = mMap.mSPU.contains(abs_addr);
		if (offset != -1)
		{
			println("Unhandled write to SPU register {:08x}: {:04x}",
					 abs_addr, val);
			return;
		}

		offset = mMap.mTIMERS.contains(abs_addr);
		if (offset != -1) {
			println("Unhandled write to timer register {:x}", offset);
			return;
		}

		offset = mMap.mIRQ_CONTROL.contains(abs_addr);
		if (offset != -1) {
			println("IRQ control write {:x}, {:04x}", offset, val);
			return;
		}

		panic("unhandled store16 into address {:08x}", addr);
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

		panic("unhandled load8 at address {:08x}", addr);
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
			println("Unhandled write to expansion 2 register {:08x}: {:02x}", abs_addr, val);
			return;
		}

		panic("unhandled store8 into address {:08x}: {:02x}", addr, val);
	}

	// DMA register read
	uint32_t Bus::dmaReg(uint32_t offset)
	{
		auto major = (offset & 0x70) >> 4;
		auto minor = offset & 0xf;

		dma::Channel *channel;

		switch (major)
		{
		// Per-channel registers
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			channel = mDma.channel(dma::port::fromIndex(major));

			switch (minor)
			{
			case 0:
				return channel->base();
			case 4:
				return channel->blockControl();
			case 8:
				return channel->control();
			default:
				panic("Unhandled DMA read at {:x}", offset);
			}
			break;
		// Common DMA registers
		case 7:
			switch (minor) {
			case 0:
				return mDma.control();
			case 4:
				return mDma.interrupt();
			default:
				panic("Unhandled DMA read at {:x}", offset);
			}
			break;
		default:
			panic("Unhandled DMA read at {:x}", offset);
		}

		return -1;
	}

	// DMA register write
	void Bus::setDmaReg(uint32_t offset, uint32_t val)
	{
		auto major = (offset & 0x70) >> 4;
		auto minor = offset & 0xf;

		dma::Port port;
		dma::Channel *channel;

		switch (major)
		{
		// Per-channel registers
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			port = dma::port::fromIndex(major);
			channel = mDma.channel(port);

			switch (minor)
			{
			case 0:
				channel->setBase(val);
				break;
			case 4:
				channel->setBlockControl(val);
				break;
			case 8:
				channel->setControl(val);
				break;
			default:
				panic("Unhandled DMA write {:x}: {:08x} (minor = {}, major = {}, mIp = {})", offset, val, minor, major, mCpu.mIp);
			}
			if (channel->active())
				doDma(port);
			break;
		// Common DMA registers
		case 7:
			switch (minor) {
			case 0:
				mDma.setControl(val);
				break;
			case 4:
				mDma.setInterrupt(val);
				break;
			default:
				panic("Unhandled DMA write {:x}: {:08x} (minor = {}, major = {}, mIp = {})", offset, val, minor, major, mCpu.mIp);
			}
			break;
		default:
			panic("Unhandled DMA write {:x}: {:08x} (minor = {}, major = {}, mIp = {})", offset, val, minor, major, mCpu.mIp);
		}
	}

	// Execute DMA transfer for a port
	void Bus::doDma(dma::Port port)
	{
		// DMA transfer has been started, for now let's
		// process everything in one pass (i.e. no
		// chopping or priority handling)

		switch (mDma.channel(port)->sync())
		{
		case dma::Sync::LinkedList:
			doDmaLinkedList(port);
			break;
		default:
			doDmaBlock(port);
		}
	}

	// Emulate DMA transfer for linked list synchronization mode.
	void Bus::doDmaLinkedList(dma::Port port)
	{
		dma::Channel *channel = mDma.channel(port);

		auto addr = channel->base() & 0x1ffffc;

		if (channel->direction() == dma::Direction::ToRam)
			panic("Invalid DMA direction for linked list mode");

		// I don't know if the DMA even supports linked list mode for
		// anything besides the GPU
		if (port != dma::Port::Gpu)
			panic("Attempted linked list DMA on port {}", (uint8_t)port);

		while (true) {
			// In linked list mode, each entry starts with a "header"
			// word. The high byte contains the number of words in the
			// "packet" (not counting the header word)
			auto header = mRam.load32(addr);

			auto remsz = header >> 24;

			if (remsz > 0)
				println("linked list packet size: {}", remsz);

			while (remsz > 0)
			{
				addr = (addr + 4) & 0x1ffffc;

				auto command = mRam.load32(addr);

				panic("GPU command {:08x}", command);

				remsz -= 1;
			}

			// The end-of-table marker is usually 0xffffff but
			// mednafen only checks for the MSB so maybe that's what
			// the hardware does? Since this bit is not part of any
			// valid address it makes some sense. I'll have to test
			// that at some point...
			if (header & 0x800000 != 0)
			{
				println("End of table");
				break;
			}

			addr = header & 0x1ffffc;
		}

		channel->done();
	}

	// Emulate DMA transfer for Manual and Request synchronization modes.
	void Bus::doDmaBlock(dma::Port port)
	{
		dma::Channel *channel = mDma.channel(port);

		uint32_t increment;
		switch (channel->step())
		{
		case dma::Step::Increment:
			increment = 4;
			break;
		case dma::Step::Decrement:
			increment = (uint32_t)(-4);
			break;
		};

		auto addr = channel->base();

		// Transfer size in words
		uint32_t remsz = channel->transferSize();
		if ((int32_t)remsz == -1) {
			// Shouldn't happen since we shouldn't be reaching this code
			// in linked list mode
			panic("Couldn't figure out DMA block transfer size");
		}

		while (remsz > 0)
		{
			// Not sure what happens if address is
			// bogus... Mednafen just masks addr this way, maybe
			// that's how the hardware behaves (i.e. the RAM
			// address wraps and the two LSB are ignored, seems
			// reasonable enough
			auto cur_addr = addr & 0x1ffffc;
			dma::Direction dir = channel->direction();

			uint32_t src_word;

			if (dir == dma::Direction::FromRam)
			{
				src_word = mRam.load32(cur_addr);

				switch (port)
				{
				case dma::Port::Gpu:
					println("GPU data {:08x}", src_word);
					break;
				default:
					panic("Unhandled DMA destination port {}", (uint8_t)port);
				}
			} else if (dir == dma::Direction::ToRam) {
				switch (port)
				{
					// Clear ordering table
					case dma::Port::Otc:
					{
						if (remsz == 1)
							// Last entry contains the end
							// of table marker
							src_word = 0xffffff;
						else
							// Pointer to the previous entry
							src_word = (addr - 4) & 0x1fffff;
						break;
					}
					default:
						panic("Unhandled DMA source port {}", (uint8_t)port);
				};

				mRam.store32(cur_addr, src_word);
			}

			addr += increment;
			remsz -= 1;
		}

		channel->done();
	}
}
