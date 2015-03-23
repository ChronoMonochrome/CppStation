#include <memory/dma.hpp>


namespace dma
{
	Dma::Dma() :
		// Reset value taken from the Nocash PSX spec
		mControl(0x07654321),
		mIrqEn(false),
		mChannelIrqEn(0),
		mChannelIrqFlags(0),
		mForceIrq(false),
		mIrqDummy(0)
	{
		for (int i = 0; i < 7; i++)
		{
			mChannels[i] = new Channel();
		}
	}

	Dma::~Dma()
	{
		for (int i = 0; i < 7; i++)
		{
			delete mChannels[i];
		}
	}

	// Return the status of the DMA interrupt
	bool Dma::irq()
	{
		auto channelIrq = mChannelIrqFlags & mChannelIrqEn;

		return mForceIrq || (mIrqEn && channelIrq != 0);
	}

	// Retreive the value of the control register
	uint32_t Dma::control()
	{
		return mControl;
	}

	// Set the value of the control register
	void Dma::setControl(uint32_t val)
	{
		mControl = val;
	}

	// Retreive the value of the interrupt register
	uint32_t Dma::interrupt()
	{
		uint32_t r = 0;

		r |= mIrqDummy;
		r |= ((uint32_t)mForceIrq) << 15;
		r |= ((uint32_t)mChannelIrqEn) << 16;
		r |= ((uint32_t)mIrqEn) << 23;
		r |= ((uint32_t)mChannelIrqFlags) << 24;
		r |= ((uint32_t)irq()) << 31;

		return r;
	}

	// Set the value of the interrupt register
	void Dma::setInterrupt(uint32_t val)
	{
		// Unknown what bits [5:0] do
		mIrqDummy = (uint8_t)(val & 0x3f);

		mForceIrq = (val >> 15) & 1 != 0;

		mChannelIrqEn = (uint8_t)((val >> 16) & 0x7f);

		mIrqEn = (val >> 23) & 1 != 0;

		// Writing 1 to a flag resets it
		uint8_t ack = (uint8_t)((val >> 24) & 0x3f);
		mChannelIrqFlags &= !ack;

		println("DMA IRQ en: {} {:08x}", mIrqEn, val);
	}

	// Return a channel pointer by port number.
	Channel *Dma::channel(Port port)
	{
		return mChannels[(uint32_t)port];
	}

	Channel::Channel() :
		mEnable(false),
		mDirection(Direction::ToRam),
		mStep(Step::Increment),
		mSync(Sync::Manual),
		mTrigger(false),
		mChop(false),
		mChopDmaSz(0),
		mChopCpuSz(0),
		mBase(0),
		mBlockSize(0),
		mBlockCount(0),
		mDummy(0)
	{
	}

	Channel::~Channel()
	{
	}

	// Retreive the channel's base address
	uint32_t Channel::base()
	{
		return mBase;
	}

	// Set channel base address. Only bits [0:23] are significant so
	// only 16MB are addressable by the DMA
	void Channel::setBase(uint32_t val)
	{
		mBase = val & 0xffffff;
	}

	// Retreive the value of the control register
	uint32_t Channel::control()
	{
		uint32_t r = 0;

		r |= ((uint32_t)mDirection) << 0;
		r |= ((uint32_t)mStep) << 1;
		r |= ((uint32_t)mChop) << 8;
		r |= ((uint32_t)mSync) << 9;
		r |= ((uint32_t)mChopDmaSz) << 16;
		r |= ((uint32_t)mChopCpuSz) << 20;
		r |= ((uint32_t)mEnable) << 24;
		r |= ((uint32_t)mTrigger) << 28;
		r |= ((uint32_t)mDummy) << 29;

		return r;
	}

	// Set the value of the control register
	void Channel::setControl(uint32_t val)
	{
		if (val & 1 != 0)
			mDirection = Direction::FromRam;
		else
			mDirection = Direction::ToRam;

		if ((val >> 1) & 1 != 0)
			mStep = Step::Decrement;
		else
			mStep = Step::Increment;

		mChop = (val >> 8) & 1 != 0;

		uint32_t n = (val >> 9) & 3;
		switch (n)
		{
		case 0:
			mSync = Sync::Manual;
			break;
		case 1:
			mSync = Sync::Request;
			break;
		case 2:
			mSync = Sync::LinkedList;
			break;
		default:
			panic("Unknown DMA sync mode {}", n);
		}

		mChopDmaSz = (uint8_t)((val >> 16) & 7);
		mChopCpuSz = (uint8_t)((val >> 20) & 7);

		mEnable = (val >> 24) & 1 != 0;
		mTrigger = (val >> 28) & 1 != 0;

		mDummy = (uint8_t)((val >> 29) & 3);
	}

	// Retreive value of the Block Control register
	uint32_t Channel::blockControl()
	{
		auto bs = (uint32_t)mBlockSize;
		auto bc = (uint32_t)mBlockCount;

		return (uint32_t)((bc << 16) | bs);
	}

	// Set value of the Block Control register
	void Channel::setBlockControl(uint32_t val)
	{
		mBlockSize = (uint16_t)val;
		mBlockCount = (uint16_t)(val >> 16);
	}

	// Return true if the channel has been started
	bool Channel::active()
	{
		bool trigger;

		// In manual sync mode the CPU must set the "trigger" bit to
		// start the transfer.
		switch (mSync)
		{
		case Sync::Manual:
			trigger = mTrigger;
			break;
		default:
			trigger = true;
		};

		return mEnable && trigger;
	}

	// Set the channel status to "completed" state
	void Channel::done() {
		mEnable = false;
		mTrigger = false;

		// XXX Need to set the correct value for the other fields (in
		// particular interrupts)
	}

	Direction Channel::direction()
	{
		return mDirection;
	}

	Step Channel::step()
	{
		return mStep;
	}

	Sync Channel::sync()
	{
		return mSync;
	}

	// Return the DMA transfer size in bytes or None for linked list
	// mode.
	int32_t Channel::transferSize()
	{
		switch (mSync)
		{
		// For manual mode only the block size is used
		case Sync::Manual:
			return mBlockSize;
		// In DMA request mode we must transfer `mBlockCount` blocks
		case Sync::Request:
			return mBlockCount * mBlockSize;
		// In linked list mode the size is not known ahead of
		// time: we stop when we encounter the "end of list"
		// marker (0xffffff)
		case Sync::LinkedList:
			return -1;
		}

		panic("unknown DMA sync mode {}", (uint32_t)mSync);
		// unreachable
		return -1;
	}

	namespace port
	{
		Port fromIndex(uint32_t index)
		{
			switch (index)
			{
			case 0:
				return Port::MdecIn;
			case 1:
				return Port::MdecOut;
			case 2:
				return Port::Gpu;
			case 3:
				return Port::CdRom;
			case 4:
				return Port::Spu;
			case 5:
				return Port::Pio;
			case 6:
				return Port::Otc;
			default:
				panic("Invalid port {}", index);
			}
		}
	}
}