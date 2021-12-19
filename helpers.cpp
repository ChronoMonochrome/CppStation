#include <iostream>

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