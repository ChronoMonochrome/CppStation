#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <fmt/core.h>

#include "result.hpp"

template<typename ... Args>
[[noreturn]] static void panic(const char *error_msg, Args ... args)
{
	std::cerr << fmt::format(error_msg, args ...) << std::endl;
	exit(-1);
}

template<typename ... Args>
[[noreturn]] static void panic(std::string error_msg, Args ... args)
{
	std::cerr << fmt::format(error_msg, args ...) << std::endl;
	exit(-1);
}

template<typename ... Args>
static void println(std::string msg, Args ... args)
{
	std::cout << fmt::format(msg, args ...) << std::endl;
}
