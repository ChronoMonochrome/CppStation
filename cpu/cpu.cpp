#include <cpu/cpu.hpp>
#include <bus.hpp>
#include <limits.h>

inline static constexpr bool AddOverflow(uint32_t old_value, uint32_t add_value, uint32_t new_value)
{
	return (((new_value ^ old_value) & (new_value ^ add_value)) & UINT32_C(0x80000000)) != 0;
}

inline static constexpr bool SubOverflow(uint32_t old_value, uint32_t sub_value, uint32_t new_value)
{
	return (((new_value ^ old_value) & (old_value ^ sub_value)) & UINT32_C(0x80000000)) != 0;
}

namespace cpu {
	Cpu::Cpu() :
		mPc(0xbfc00000), // PC reset value at the beginning of the BIOS
		mNextInstruction(0x0), // NOP
		mSr(0)
	{
		for (int i = 1; i < 32; i++)
			mRegs[i] = 0xdeadc0de;

		mRegs[0] = 0;
	}

	uint32_t Cpu::reg(registerIndex index)
	{
		return mRegs[index.val];
	}

	void Cpu::setReg(registerIndex index, uint32_t val)
	{
		mRegs[index.val] = val;
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
			case 0b100101:
				opOr(instruction);
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
		case 0b000010:
			opJ(instruction);
			break;
		case 0b010000:
			opCop0(instruction);
			break;
		case 0b000101:
			opBne(instruction);
			break;
		case 0b001000:
			opAddi(instruction);
			break;
		default:
			panic(fmt::format("Unhandled instruction {:x}", instruction.mData));
		}
	}

	void Cpu::runNextInstruction()
	{
		uint32_t pc = mPc;

		// Use previously loaded instruction
		Instruction instruction(mNextInstruction);

		// Fetch instruction at PC
		mNextInstruction = Instruction(load32(pc));

		// Increment PC to point to the next instruction. All
		// instructions are 32bit long.
		mPc = pc + 4;
#ifdef DEBUG
		cout << fmt::format("instruction: {:x}", instruction.mData) << endl;
#endif

		decodeAndExecute(instruction);
	}

	void Cpu::opLui(Instruction &instruction)
	{
		auto i = instruction.imm();
		auto t = instruction.t();

		// Low 16bits are set to 0
		auto v = i << 16;
		setReg(t, v);
	}

	void Cpu::opOri(Instruction &instruction)
	{
		auto i = instruction.imm();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = reg(s) | i;
		setReg(t, v);
	}

	void Cpu::opSw(Instruction &instruction)
	{
		if ((mSr & 0x10000) != 0) {
			// Cache is isolated, ignore write
			println("Ignoring store while cache is isolated");
			return;
		}

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;
		auto v = reg(t);
		store32(addr, v);
	}

	void Cpu::opSll(Instruction &instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = reg(t) << i;

		setReg(d, v);
	}

	void Cpu::opAddiu(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = reg(s) + i;

		setReg(t, v);
	}

	void Cpu::opJ(Instruction &instruction)
	{
		auto i = instruction.imm_jump();
		mPc = (mPc & 0xf0000000) | (i << 2);
	}

	void Cpu::opOr(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = reg(s) | reg(t);

		setReg(d, v);
	}

	void Cpu::opCop0(Instruction &instruction)
	{
		switch (instruction.copOpcode()) {
		case 0b00100:
			opMtc0(instruction);
			break;
		default:
			panic(fmt::format("unhandled cop0 instruction {}",
						  instruction.mData));
		}
	}

	void Cpu::opMtc0(Instruction &instruction)
	{
		auto cpu_r = instruction.t();
		auto cop_r = instruction.d().val;

		auto v = reg(cpu_r);

		switch (cop_r) {
		case 3:
		case 5:
		case 6:
		case 7:
		case 9:
		case 11: // Breakpoints registers
			if (v != 0)
				panic(fmt::format("Unhandled write to cop0r{}", cop_r));
			break;
		case 12:
			mSr = v;
			break;
		case 13: // Cause register
			if (v != 0)
				panic("Unhandled write to CAUSE register.");
			break;
		default:
			panic(fmt::format("Unhandled cop0 register {}", cop_r));
		}
	}

	void Cpu::branch(uint32_t offset)
	{
		// Offset immediates are always shifted two places to the
		// right since `PC` addresses have to be aligned on 32bits at
		// all times.
		offset = offset << 2;

		mPc += offset;

		// We need to compensate for the hardcoded
		// `pc.wrapping_add(4)` in `run_next_instruction`
		mPc -= 4;
	}


	void Cpu::opBne(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();
		auto t = instruction.t();

		if (reg(s) != reg(t))
		{
			branch(i);
		}
	}

	void Cpu::opAddi(Instruction &instruction)
	{
		uint32_t i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();
		uint32_t s1 = reg(s);
		uint32_t v = s1 + i;

		if (AddOverflow(s1, i, v))
			panic("ADDI overflow");

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

	// Return coprocessor opcode in bits [25:21]
	uint32_t Instruction::copOpcode()
	{
		return (mData >> 21) & 0x1f;
	}

	// Return register index in bits [20:16]
	registerIndex Instruction::t()
	{
		registerIndex ret;
		ret.val = (mData >> 16) & 0x1f;
		return ret;
	}

	// Return register index in bits [25:21]
	registerIndex Instruction::s()
	{
		registerIndex ret;
		ret.val = (mData >> 21) & 0x1f;
		return ret;
	}

	// Return register index in bits [15:11]
	registerIndex Instruction::d()
	{
		registerIndex ret;
		ret.val = (mData >> 11) & 0x1f;
		return ret;
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

	// Jump target stored in bits [25:0]
	uint32_t Instruction::imm_jump()
	{
		return mData & 0x3ffffff;
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
