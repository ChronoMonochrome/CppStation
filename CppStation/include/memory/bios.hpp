#pragma once

#include "helpers.hpp"

namespace bus {
class Bus;
}

namespace bios {

const uint64_t BIOS_SIZE = 512 * 1024;
class Bios
{
public:
	Bios();
	auto loadFromFile(std::string &path) -> cpp::result<uint64_t, std::string>;
	uint32_t load32(size_t offset);
	uint8_t load8(size_t offset);
	~Bios();
	bool mIsValidImage = false;
public:
	char *mBuffer;
	// Linkage to the communications bus
	bus::Bus *mBus = nullptr;
	// Link this CPU to a communications bus
	void connectBus(bus::Bus *n) { mBus = n; }
};

} // namespace bios