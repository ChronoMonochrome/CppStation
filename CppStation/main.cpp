#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>
#include "helpers.hpp"
#include "backtrace.hpp"

int main()
{
	setupSigAct();
	bus::Bus bus;
	
	while (true)
	{
		bus.mCpu.runNextInstruction();
	}

	return 0;
}
