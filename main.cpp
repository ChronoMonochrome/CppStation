#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>
#include "helpers.hpp"

int main()
{
	bus::Bus bus;
	
	while (true)
	{
		bus.mCpu.runNextInstruction();
	}

	return 0;
}
