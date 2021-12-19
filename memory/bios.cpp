#include <memory/bios.hpp>

namespace bios {
	const uint64_t BIOS_SIZE = 512 * 1024;

	Bios::Bios(std::string &path)
	{
		std::ifstream stream(path, std::ios::in | std::ios::binary);
		std::vector<uint8_t> mBuffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		if (mBuffer.size() == BIOS_SIZE)
			mIsValidImage = true;
	}

	Bios::~Bios()
	{
	}
	
	auto getBios() noexcept -> cpp::result<Bios*, std::string>
	{
		std::string path("roms/SCPH1001.BIN");

		Bios* mybios = new Bios(path);
		if (errno != 0) {
			return cpp::fail(path + ": " + std::string(std::strerror(errno)));
		}
		
		if (!mybios->mIsValidImage) {
			return cpp::fail(path + " is not a valid bios image");
		}
		
		return mybios;
	}
}