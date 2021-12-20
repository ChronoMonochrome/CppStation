#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

#include "result.hpp"
#include <fmt/core.h>

[[noreturn]] void panic(const char *error_msg);
[[noreturn]] void panic(std::string error_msg);
void println(std::string msg);