#pragma once

#include <memory/bios.hpp>
#include <memory/ram.hpp>
#include <cstdint>

namespace map {
	class Range {
	public:
		Range(uint32_t base, uint32_t size);
		int32_t contains(uint32_t addr);
		~Range();
	private:
		uint32_t mBase, mSize, mEnd;
	};

	class Map {
	public:
		Map();
		~Map();
		Range mBIOS;
		Range mMEM_CONTROL;
		Range mRAM_SIZE;
		Range mCACHE_CONTROL;
		Range mRAM;
	};
}