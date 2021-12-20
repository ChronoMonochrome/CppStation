#include "helpers.hpp"

using namespace std;

[[noreturn]] void panic(const char *error_msg)
{
	cerr << error_msg << endl;
	//quick_exit(0);
	exit(-1);
}

[[noreturn]] void panic(std::string error_msg)
{
	cerr << error_msg << endl;
	//quick_exit(0);
	exit(-1);
}

template<typename ... Args>
static std::string string_format(const std::string& format, Args ... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0)
		throw std::runtime_error("Error during formatting.");

    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}