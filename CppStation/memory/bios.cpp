#include <memory/bios.hpp>
#include <bus.hpp>

using namespace std;

namespace bios {

Bios::Bios()
{
}

Bios::~Bios()
{
}

auto Bios::loadFromFile(std::string &path) -> cpp::result<uint64_t, std::string>
{
	std::ifstream ifs;
	ifs.open(path, std::ifstream::binary);
	if (ifs.is_open())
	{
		ifs.seekg(0, std::ios_base::end);
		uint64_t bufferSize = ifs.tellg();
		if (bufferSize == BIOS_SIZE) {
			mIsValidImage = true;
			ifs.seekg(0, std::ios_base::beg);
			mBuffer.resize(bufferSize);
			ifs.read((char*)mBuffer.data(), mBuffer.size());
		}
		return bufferSize;
	}
	ifs.close();

	if (errno != 0) {
		return cpp::fail(path + ": " + std::string(std::strerror(errno)));
	}

	return cpp::fail(path + " is not a valid bios image");
}

// Fetch byte at `offset`
uint8_t Bios::load8(size_t offset)
{
	return mBuffer[offset];
}

// Fetch the 32 bit little endian word at ‘offset‘
uint32_t Bios::load32(size_t offset)
{
	uint8_t b0 = mBuffer[offset + 0];
	uint8_t b1 = mBuffer[offset + 1];
	uint8_t b2 = mBuffer[offset + 2];
	uint8_t b3 = mBuffer[offset + 3];
	return b0 | ( b1 << 8 ) | ( b2 << 16 ) | ( b3 << 24 );
}

} // namespace bios