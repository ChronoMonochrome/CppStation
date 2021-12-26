#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>

namespace bus {
	class Bus;
}

namespace ram {
	const uint64_t RAM_SIZE = 2 * 1024 * 1024;

	class Ram {
	public:
		Ram();
		~Ram();
		// Fetch the 32bit little endian word at `offset`
		uint32_t load32(size_t offset);
		// Store the 32bit little endian word `val` into `offset`
		void store32(size_t offset, uint32_t val);
		// Fetch the byte at `offset`
		uint8_t load8(size_t offset);
		// Store the byte `val` into `offset`
		void store8(size_t offset, uint8_t val);
		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link RAM to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	private:
		std::vector<uint8_t> mData;
	};
}