#include <memory>
#include <fstream>
#include <iterator>
#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>
#include "helpers.hpp"

namespace bios {
	class Bios
	{
	public:
		Bios(std::string &path);
		~Bios();
		bool mIsValidImage = false;
	private:
		std::vector<uint8_t> mBuffer;
	};
	
	auto getBios() noexcept -> cpp::result<Bios*, std::string>;
}