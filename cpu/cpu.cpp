#include <cpu/cpu.hpp>
#include <bus.hpp>

namespace cpu {
	Cpu::Cpu() :
		mPc(0xbfc00000) // PC reset value at the beginning of the BIOS
	{
		for (int i = 1; i < 32; i++)
			mRegs[i] = 0xdeadc0de;

		mRegs[0] = 0;
	}

	uint32_t Cpu::reg(uint32_t index)
	{
		return mRegs[index];
	}

	void Cpu::setReg(uint32_t index, uint32_t val)
	{
		mRegs[index] = val;
		// R0 is always set to 0
		mRegs[0] = 0;
	}

	uint32_t Cpu::load32(uint32_t addr)
	{
		return mBus->load32(addr);
	}

	void Cpu::store32(uint32_t addr, uint32_t val)
	{
		mBus->store32(addr, val);
	}

	void Cpu::decodeAndExecute(Instruction &instruction)
	{
		switch (instruction.function())
		{
		case 0b000000:
			switch (instruction.subfunction())
			{
			case 0b000000:
				opSll(instruction);
				break;
			default:
				panic(fmt::format("Unhandled instruction {:x}", instruction.mData));
			}
			break;
		case 0b001111:
			opLui(instruction);
			break;
		case 0b001101:
			opOri(instruction);
			break;
		case 0b101011:
			opSw(instruction);
			break;
		case 0b001001:
			opAddiu(instruction);
			break;
		default:
			panic(fmt::format("Unhandled instruction {:x}", instruction.mData));
		}
	}

	void Cpu::runNextInstruction()
	{
		Instruction instruction(load32(mPc));
		mPc = (uint32_t)(mPc + 4);
		decodeAndExecute(instruction);
	}

	void Cpu::opLui(Instruction &instruction)
	{
		uint32_t i = instruction.imm();
		uint32_t t = instruction.t();

		// Low 16bits are set to 0
		uint32_t v = i << 16;
		setReg(t, v);
	}

	void Cpu::opOri(Instruction &instruction)
	{
		uint32_t i = instruction.imm();
		uint32_t t = instruction.t();
		uint32_t s = instruction.s();

		uint32_t v = reg(s) | i;
		setReg(t, v);
	}

	void Cpu::opSw(Instruction &instruction)
	{
		uint32_t i = instruction.imm_se();
		uint32_t t = instruction.t();
		uint32_t s = instruction.s();

		uint32_t addr = (uint32_t)(reg(s) + i);
		uint32_t v = reg(t);
		store32(addr, v);
	}

    // Shift Left Logical
    void Cpu::opSll(Instruction &instruction)
	{
        uint32_t i = instruction.shift();
        uint32_t t = instruction.t();
        uint32_t d = instruction.d();

        uint32_t v = reg(t) << i;

        setReg(d, v);
    }

    // Add Immediate Unsigned
    void Cpu::opAddiu(Instruction &instruction)
	{
        uint32_t i = instruction.imm_se();
        uint32_t t = instruction.t();
        uint32_t s = instruction.s();

        uint32_t v = (uint32_t)(reg(s) + i);

        setReg(t, v);
    }

	Cpu::~Cpu()
	{
	}

	Instruction::Instruction(uint32_t data) : mData(data)
	{
	}

	// Return bits [31:26] of the instruction
	uint32_t Instruction::function()
	{
		return mData >> 26;
	}

	// Return bits [5:0] of the instruction
	uint32_t Instruction::subfunction()
	{
		return mData & 0x3f;
	}

	// Return register index in bits [20:16]
	uint32_t Instruction::t()
	{
		return (mData >> 16) & 0x1f;
	}

	// Return register index in bits [25:21]
	uint32_t Instruction::s()
	{
		return (mData >> 21) & 0x1f;
	}

	// Return register index in bits [15:11]
	uint32_t Instruction::d()
	{
		return (mData >> 11) & 0x1f;
	}

	// Return immediate value in bits [16:0]
	uint32_t Instruction::imm()
	{
		return mData & 0xffff;
	}

	// Return immediate value in bits [16:0] as a sign-extended 32bit value
	uint32_t Instruction::imm_se()
	{
		int16_t tmp = mData & 0xffff;
		return (uint32_t)tmp;
	}

	// Shift Immediate values are stored in bits [10:6]
	uint32_t Instruction::shift()
	{
		return (mData >> 6) & 0x1f;
	}


	Instruction::~Instruction()
	{
	}
}
