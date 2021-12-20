#include <iostream>

#include <memory/bios.hpp>
#include <cpu/cpu.hpp>
#include <bus.hpp>
#include "helpers.hpp"

class Demo
{
public:
	bus::Bus bus;

	Demo()
	{
		
		//auto res = bios::getBios();
		
		//interconnect.addBios(bios);
		//cpu.addInterconnect(interconnect);
		
		cout << "buffer size " << bus.mBios.mBuffer.size() << endl;
		//cout << "instruction " << int(bus.mBios.mBuffer[0]) << endl;
		//cpu->run_next_instruction();
		bus.mCpu.runNextInstruction();
	}
	~Demo()
	{
	}
};

int main()
{
	Demo *demo = new Demo();
	delete demo;

	return 0;
}
