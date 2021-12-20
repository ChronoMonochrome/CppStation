#include <cpu/cpu.hpp>
#include <bus.hpp>

namespace cpu {
	Cpu::Cpu() :
		mPc(0xbfc00000) // PC reset value at the beginning of the BIOS
	{
	}
	
	uint32_t Cpu::load32(uint32_t addr)
	{
		return mBus->load32(addr);
	}
	
	void Cpu::decodeAndExecute(uint32_t instruction)
	{
		panic(fmt::format("Unhandled instruction {:x}", instruction));
	}
	
	void Cpu::runNextInstruction()
	{
		uint32_t instruction = load32(mPc);
		mPc = (uint32_t)(mPc + 4);
		decodeAndExecute(instruction);
	}

	Cpu::~Cpu()
	{
	}
}
