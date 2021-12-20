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
		void loadFromFile(std::string &path);
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