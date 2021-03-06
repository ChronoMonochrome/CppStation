#include <memory/ram.hpp>
#include "helpers.hpp"

namespace ram {

Ram::Ram()
{
	mData = (char*)malloc(RAM_SIZE);
	if (!mData)
		panic("Not enough memory to allocate RAM buffer");

	// Default RAM contents are garbage
	memset(mData, 0xca, RAM_SIZE);
}

Ram::~Ram()
{
	free(mData);
}

// Fetch the 32 bit little endian word at ‘offset‘
uint32_t Ram::load32(size_t offset)
{
	uint8_t b0 = mData[offset + 0];
	uint8_t b1 = mData[offset + 1];
	uint8_t b2 = mData[offset + 2];
	uint8_t b3 = mData[offset + 3];

	return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

// Store the 32bit little endian word `val` into `offset`
void Ram::store32(size_t offset, uint32_t val)
{
	uint8_t b0 = val & 0xff;
	uint8_t b1 = (val >> 8) & 0xff;
	uint8_t b2 = (val >> 16) & 0xff;
	uint8_t b3 = (val >> 24) & 0xff;

	mData[offset + 0] = b0;
	mData[offset + 1] = b1;
	mData[offset + 2] = b2;
	mData[offset + 3] = b3;
}

// Fetch the 16bit little endian halfword at `offset`
uint16_t Ram::load16(size_t offset)
{
	uint8_t b0 = mData[offset + 0];
	uint8_t b1 = mData[offset + 1];

	return b0 | (b1 << 8);
}

// Store the 16bit little endian halfword `val` into `offset`
void Ram::store16(size_t offset, uint16_t val)
{
	uint8_t b0 = val & 0xff;
	uint8_t b1 = (val >> 8) & 0xff;

	mData[offset + 0] = b0;
	mData[offset + 1] = b1;
}

// Fetch the byte at `offset`
uint8_t Ram::load8(size_t offset)
{
	return mData[offset];
}

// Store the byte `val` into `offset`
void Ram::store8(size_t offset, uint8_t val)
{
	mData[offset] = val;
}
} // namespace ram