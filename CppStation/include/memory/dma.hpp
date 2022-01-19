#pragma once

#include <vector>
#include "helpers.hpp"

namespace bus {
class Bus;
}

namespace dma {

// DMA transfer direction
enum class Direction
{
	ToRam   = 0,
	FromRam = 1,
};

// DMA transfer step
enum class Step
{
	Increment = 0,
	Decrement = 1,
};

// DMA transfer synchronization mode
enum class Sync
{
	// Transfer starts when the CPU writes to the Trigger bit and
	// transfers everything at once
	Manual = 0,
	// Sync blocks to DMA requests
	Request = 1,
	// Used to transfer GPU command lists
	LinkedList = 2,
};

// The 7 DMA ports
enum class Port {
	// Macroblock decoder input
	MdecIn = 0,
	// Macroblock decoder output
	MdecOut = 1,
	// Graphics Processing Unit
	Gpu = 2,
	// CD-ROM drive
	CdRom = 3,
	// Sound Processing Unit
	Spu = 4,
	// Extension port
	Pio = 5,
	// Used to clear the ordering table
	Otc = 6,
};

namespace port
{
	Port fromIndex(uint32_t index);
}

class Channel
{
public:
	Channel();
	~Channel();
public:
	// If true the channel is enabled and the copy can take place
	// depending on the condition mandated by the `sync` mode.
	bool mEnable;
	// Copy direction: from RAM or from device
	Direction mDirection;
	// DMA can either increment or decrement the RAM pointer after
	// each copy
	Step mStep;
	// Synchronization mode
	Sync mSync;
	// Used to start the DMA transfer when `sync` is `Manual`
	bool mTrigger;
	// If true the DMA "chops" the transfer and lets the CPU run in
	// the gaps.
	bool mChop;
	// Chopping DMA window size (log2 number of words)
	uint8_t mChopDmaSz;
	// Chopping CPU window size (log2 number of cycles)
	uint8_t mChopCpuSz;
	// DMA start address
	uint32_t mBase;
	// Size of a block in words
	uint16_t mBlockSize;
	// Block count, only used when `sync` is `Request`
	uint16_t mBlockCount;
	// Unknown 2 RW bits in configuration register
	uint8_t mDummy;

	// Retreive the channel's base address
	uint32_t base();

	// Set channel base address. Only bits [0:23] are significant so
	// only 16MB are addressable by the DMA
	void setBase(uint32_t val);

	// Retreive the value of the control register
	uint32_t control();

	// Set the value of the control register
	void setControl(uint32_t val);

	// Retreive value of the Block Control register
	uint32_t blockControl();

	// Set value of the Block Control register
	void setBlockControl(uint32_t val);

	// Return true if the channel has been started
	bool active();

	// Set the channel status to "completed" state
	void done();

	Direction direction();

	Step step();

	Sync sync();

	// Return the DMA transfer size in bytes or -1 for linked list mode.
	int32_t transferSize();
};

class Dma
{
public:
	Dma();
	~Dma();
public:
	// DMA control register
	uint32_t mControl;
	// master IRQ enable
	bool mIrqEn;
	// IRQ enable for individual channels
	uint8_t mChannelIrqEn;
	// IRQ flags for individual channels
	uint8_t mChannelIrqFlags;
	// When set the interrupt is active unconditionally (even if
	// `irq_en` is false)
	bool mForceIrq;
	// Bits [0:5] of the interrupt registers are RW but I don't know
	// what they're supposed to do so I just store them and send them
	// back untouched on reads
	uint8_t mIrqDummy;
	// The 7 channel instances
	Channel *mChannels[7];

	// Return the status of the DMA interrupt
	bool irq();

	// Retreive the value of the control register
	uint32_t control();

	// Set the value of the control register
	void setControl(uint32_t val);

	// Retreive the value of the interrupt register
	uint32_t interrupt();

	// Set the value of the interrupt register
	void setInterrupt(uint32_t val);

	// Return a reference to a channel by port number.
	Channel *channel(Port port);

	// Linkage to the communications bus
	bus::Bus *mBus = nullptr;
	// Link DMA to a communications bus
	void connectBus(bus::Bus *n) { mBus = n; }
	};

} // namespace dma