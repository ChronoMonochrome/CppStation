#pragma once

#include <memory>
#include <fstream>
#include <iterator>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>

#include "helpers.hpp"

namespace bus {
	class Bus;
}

namespace bios {
	class Bios
	{
	public:
		Bios();
		auto loadFromFile(std::string &path) -> cpp::result<uint64_t, std::string>;
		uint32_t load32(size_t offset);
		~Bios();
		bool mIsValidImage = false;
	public:
		std::vector<uint8_t> mBuffer;
		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link this CPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}