#ifndef _HELPERS_CPP
#define _HELPERS_CPP

#include "result.hpp"

[[noreturn]] void panic(const char *error_msg);
[[noreturn]] void panic(std::string error_msg);
#endif