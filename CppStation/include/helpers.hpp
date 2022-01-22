#pragma once

#include <iostream>
#include <iterator>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include <cstdint>
#include <cstddef>
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

static std::string readFile(const char* filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	} else {
		println("Could not open file: '{}'", filepath);
	}

	return result;
}