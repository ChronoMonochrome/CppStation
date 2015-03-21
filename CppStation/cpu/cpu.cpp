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
		mIp(0),
		mNextPc(mPc + 4),
		mCurrentPc(0),
		mLoadRegIdx(0),
		mLoadReg(0),
		mSr(0),
		mCause(0),
		mEpc(0),
		mHi(0xdeadc0de),
		mLo(0xdeadc0de),
		mBranch(false),
		mDelaySlot(false)
	{
		mLoadRegIdx.val = 0;
		for (int i = 1; i < 32; i++)
		{
			mRegs[i] = 0xdeadc0de;
			mOutRegs[i] = mRegs[i];
		}

		mRegs[0] = 0;
		mOutRegs[0] = 0;
	}

	uint32_t Cpu::reg(RegisterIndex index)
	{
		return mRegs[index.val];
	}

	void Cpu::setReg(RegisterIndex index, uint32_t val)
	{
		mOutRegs[index.val] = val;
		// R0 is always set to 0
		mOutRegs[0] = 0;
	}

	uint32_t Cpu::load32(uint32_t addr)
	{
		return mBus->load32(addr);
	}

	void Cpu::store32(uint32_t addr, uint32_t val)
	{
		if ((mSr & 0x10000) != 0) {
			// Cache is isolated, ignore write
			println("Ignoring store while cache is isolated");
			return;
		}

		mBus->store32(addr, val);
	}

	uint16_t Cpu::load16(uint32_t addr)
	{
		return mBus->load16(addr);
	}

	void Cpu::store16(uint32_t addr, uint16_t val)
	{
		if ((mSr & 0x10000) != 0) {
			// Cache is isolated, ignore write
			println("Ignoring store while cache is isolated");
			return;
		}

		mBus->store16(addr, val);
	}

	uint8_t Cpu::load8(uint32_t addr)
	{
		return mBus->load8(addr);
	}

	void Cpu::store8(uint32_t addr, uint8_t val)
	{
		if ((mSr & 0x10000) != 0)
		{
			// Cache is isolated, ignore write
			println("Ignoring store while cache is isolated");
			return;
		}
		mBus->store8(addr, val);
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
			case 0b101011:
				opSltu(instruction);
				break;
			case 0b100001:
				opAddu(instruction);
				break;
			case 0b001000:
				opJr(instruction);
				break;
			case 0b100100:
				opAnd(instruction);
				break;
			case 0b100000:
				opAdd(instruction);
				break;
			case 0b001001:
				opJalr(instruction);
				break;
			case 0b100011:
				opSubu(instruction);
				break;
			case 0b000011:
				opSra(instruction);
				break;
			case 0b011010:
				opDiv(instruction);
				break;
			case 0b010010:
				opMflo(instruction);
				break;
			case 0b000010:
				opSrl(instruction);
				break;
			case 0b011011:
				opDivu(instruction);
				break;
			case 0b010000:
				opMfhi(instruction);
				break;
			case 0b101010:
				opSlt(instruction);
				break;
			case 0b001100:
				opSyscall(instruction);
				break;
			case 0b010001:
				opMthi(instruction);
				break;
			case 0b010011:
				opMtlo(instruction);
				break;
			case 0b000100:
				opSllv(instruction);
				break;
			case 0b100111:
				opNor(instruction);
				break;
			case 0b000111:
				opSrav(instruction);
				break;
			case 0b000110:
				opSrlv(instruction);
				break;
			case 0b011001:
				opMultu(instruction);
				break;
			case 0b100110:
				opXor(instruction);
				break;
			case 0b001101:
				opBreak(instruction);
				break;
			case 0b011000:
				opMult(instruction);
				break;
			case 0b100010:
				opSub(instruction);
				break;
			default:
				opIllegal(instruction);
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
		case 0b100011:
			opLw(instruction);
			break;
		case 0b101001:
			opSh(instruction);
			break;
		case 0b000011:
			opJal(instruction);
			break;
		case 0b001100:
			opAndi(instruction);
			break;
		case 0b101000:
			opSb(instruction);
			break;
		case 0b100000:
			opLb(instruction);
			break;
		case 0b000100:
			opBeq(instruction);
			break;
		case 0b000110:
			opBlez(instruction);
			break;
		case 0b000111:
			opBgtz(instruction);
			break;
		case 0b100100:
			opLbu(instruction);
			break;
		case 0b000001:
			opBxx(instruction);
			break;
		case 0b001010:
			opSlti(instruction);
			break;
		case 0b001011:
			opSltiu(instruction);
			break;
		case 0b100101:
			opLhu(instruction);
			break;
		case 0b100001:
			opLh(instruction);
			break;
		case 0b001110:
			opXori(instruction);
			break;
		case 0b010001:
			opCop1(instruction);
			break;
		case 0b010010:
			opCop2(instruction);
			break;
		case 0b010011:
			opCop3(instruction);
			break;
		case 0b100010:
			opLwl(instruction);
			break;
		case 0b100110:
			opLwr(instruction);
			break;
		case 0b101010:
			opSwl(instruction);
			break;
		case 0b101110:
			opSwr(instruction);
			break;
		case 0b110000:
			opLwc0(instruction);
			break;
		case 0b110001:
			opLwc1(instruction);
			break;
		case 0b110010:
			opLwc2(instruction);
			break;
		case 0b110011:
			opLwc3(instruction);
			break;
		case 0b111000:
			opSwc0(instruction);
			break;
		case 0b111001:
			opSwc1(instruction);
			break;
		case 0b111010:
			opSwc2(instruction);
			break;
		case 0b111011:
			opSwc3(instruction);
			break;
		default:
			opIllegal(instruction);
		}
	}

	void Cpu::runNextInstruction()
	{
		// Save the address of the current instruction to save in
		// `EPC` in case of an exception.
		mCurrentPc = mPc;

		if (mCurrentPc % 4 != 0)
		{
			// PC is not correctly aligned!
			exception(exception::LoadAddressError);
			return;
		}

		// Fetch instruction at PC
		Instruction instruction(load32(mPc));
#ifdef DEBUG
		if (mIp >= 2695640)
			println("{} instruction: {:08x} pc={:08x} mNextPc={:08x} mCurrentPc={:08x}", mIp, instruction.mData, mPc, mNextPc, mCurrentPc);
#endif

		// Increment PC to point to the next instruction. and
		// `next_pc` to the one after that. Both values can be
		// modified by individual instructions (`next_pc` in case of a
		// jump/branch, `pc` in case of an exception)
		mPc		= mNextPc;
		mNextPc	= mPc + 4;

		// Execute the pending load (if any, otherwise it will load
		// `R0` which is a NOP). `set_reg` works only on `out_regs`
		// so this operation won't be visible by the next
		// instruction.
		RegisterIndex reg = mLoadRegIdx;
		uint32_t val = mLoadReg;
		setReg(reg, val);

		// We reset the load to target register 0 for the next
		// instruction
		mLoadRegIdx.val = 0;
		mLoadReg = 0;

		// If the last instruction was a branch then we're in the
		// delay slot
		mDelaySlot    = mBranch;
		mBranch       = false;

		decodeAndExecute(instruction);

		// Copy the output registers as input for the
		// next instruction
		for (int i = 0; i < 32; i++)
		{
			mRegs[i] = mOutRegs[i];
		}
		mIp++;
	}

	void Cpu::exception(enum exception::Exception cause)
	{
		uint32_t handler;
		// Exception handler address depends on the `BEV` bit:
		if ((mSr & (1 << 22)) != 0)
			handler = 0xbfc00180;
		else
			handler = 0x80000080;

		// Shift bits [5:0] of `SR` two places to the left. Those bits
		// are three pairs of Interrupt Enable/User Mode bits behaving
		// like a stack 3 entries deep. Entering an exception pushes a
		// pair of zeroes by left shifting the stack which disables
		// interrupts and puts the CPU in kernel mode. The original
		// third entry is discarded (it's up to the kernel to handle
		// more than two recursive exception levels).
		auto mode = mSr & 0x3f;
		mSr &= !0x3f;
		mSr |= (mode << 2) & 0x3f;

		// Update `CAUSE` register with the exception code (bits
		// [6:2])
		mCause = ((uint32_t)cause) << 2;

		// Save current instruction address in `EPC`
		mEpc = mCurrentPc;

		if (mDelaySlot)
		{
			// When an exception occurs in a delay slot `EPC` points
			// to the branch instruction and bit 31 of `CAUSE` is set.
			mEpc -= 4;
			mCause |= 1 << 31;
		}

		// Exceptions don't have a branch delay, we jump directly into
		// the handler
		mPc     = handler;
		mNextPc = mPc + 4;
	}

	void Cpu::opSyscall(Instruction &instruction)
	{
		exception(exception::SysCall);
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

		// Address must be 32bit aligned
		if (addr % 4 == 0)
			store32(addr, v);
		else
			exception(exception::StoreAddressError);
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
		mNextPc = (mNextPc & 0xf0000000) | (i << 2);

		mBranch = true;
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
		case 0b00000:
			opMfc0(instruction);
			break;
		case 0b00100:
			opMtc0(instruction);
			break;
		case 0b10000:
			opRfe(instruction);
			break;
		default:
			panic("unhandled cop0 instruction {:08x}", instruction.mData);
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
				panic("Unhandled write to cop0r{}", cop_r);
			break;
		case 12:
			mSr = v;
			break;
		case 13: // Cause register
			if (v != 0)
				panic("Unhandled write to CAUSE register.");
			break;
		default:
			panic("Unhandled cop0 register {}", cop_r);
		}
	}

	void Cpu::branch(uint32_t offset)
	{
		// Offset immediates are always shifted two places to the
		// right since `PC` addresses have to be aligned on 32bits at
		// all times.
		offset = offset << 2;

		mNextPc = mPc + offset;

		mBranch = true;
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
			exception(exception::Overflow);

		setReg(t, v);
	}

	void Cpu::opLw(Instruction &instruction) {

		if ((mSr & 0x10000) != 0)
		{
			// Cache is isolated, ignore write
			println("Ignoring load while cache is isolated");
			return;
		}

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;

		auto v = load32(addr);

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opSltu(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = reg(s) < reg(t);

		setReg(d, v);
	}

	void Cpu::opAddu(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = reg(s) + reg(t);

		setReg(d, v);
	}

	void Cpu::opSh(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;
		uint16_t v = (uint16_t)reg(t);

		// Address must be 16bit aligned
		if (addr % 2 == 0)
			store16(addr, v);
		else
			exception(exception::StoreAddressError);
	}

	void Cpu::opJal(Instruction &instruction)
	{
		uint32_t ra = mNextPc;

		// Store return address in $31 ($ra)
		RegisterIndex regIdx(31);

		setReg(regIdx, ra);

		opJ(instruction);

		mBranch = true;
	}

	void Cpu::opAndi(Instruction &instruction)
	{
		auto i = instruction.imm();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = reg(s) & i;

		setReg(t, v);
	}

	void Cpu::opSb(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;
		auto v = reg(t);

		store8(addr, v);
	}

	void Cpu::opJr(Instruction &instruction)
	{
		auto s = instruction.s();

		mNextPc = reg(s);

		mBranch = true;
	}

	void Cpu::opLb(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;

		// Cast as i8 to force sign extension
		int8_t v = (int8_t)load8(addr);

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opBeq(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();
		auto t = instruction.t();

		if (reg(s) == reg(t))
		{
			branch(i);
		}
	}

	void Cpu::opMfc0(Instruction &instruction)
	{
		auto cpuR = instruction.t();
		auto copR = instruction.d().val;
		uint32_t v;

		switch (copR)
		{
		case 12:
			v = mSr;
			break;
		case 13:
			v = mCause;
			break;
		case 14:
			v = mEpc;
			break;
		default:
			panic("Unhandled read from cop0r{}", copR);
		}

		mLoadRegIdx.val = cpuR.val;
		mLoadReg = v;
	}

	void Cpu::opAnd(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = reg(s) & reg(t);

		setReg(d, v);
	}

	void Cpu::opAdd(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();
		auto d = instruction.d();

		uint32_t i_s = reg(s);
		uint32_t i_t = reg(t);

		uint32_t v = i_s + i_t;

		if (AddOverflow(i_s, i_t, v))
			exception(exception::Overflow);

		setReg(d, v);
	}

	void Cpu::opBgtz(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();

		int32_t v = reg(s);

		if (v > 0)
			branch(i);
	}

	void Cpu::opBlez(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();

		int32_t v = reg(s);

		if (v <= 0)
			branch(i);
	}

	void Cpu::opLbu(Instruction &instruction) {

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;

		auto v = load8(addr);

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opJalr(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();

		uint32_t ra = mNextPc;

		// Store return address in `d`
		setReg(d, ra);

		mNextPc = reg(s);

		mBranch = true;
	}

	void Cpu::opBxx(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();

		uint32_t instruction_val = instruction.mData;

		bool is_bgez = (instruction_val >> 16) & 1;
		bool is_link = ((instruction_val >> 17) & 0xf) == 8;

		int32_t v = reg(s);

		// Test "less than zero"
		uint32_t test = (v < 0);

		// If the test is "greater than or equal to zero" we need
		// to negate the comparison above since
		// ("a >= 0" <=> "!(a < 0)"). The xor takes care of that.
		test = test ^ (uint32_t)is_bgez;

		if (is_link)
		{
			uint32_t ra = mNextPc;

			// Store return address in R31
			setReg(RegisterIndex(31), ra);
		}

		if (test != 0)
			branch(i);
	}

	void Cpu::opSlti(Instruction &instruction)
	{
		int32_t i = instruction.imm_se();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = ((int32_t)reg(s)) < i;

		setReg(t, v);
	}

	void Cpu::opSubu(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = reg(s) - reg(t);

		setReg(d, v);
	}

	void Cpu::opSra(Instruction &instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.t();
		auto d = instruction.d();

		uint32_t v = ((int32_t)reg(t)) >> i;

		setReg(d, v);
	}

	void Cpu::opDiv(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		int32_t n = reg(s);
		int32_t d = reg(t);

		if (d == 0)
		{
			// Division by zero, results are bogus
			mHi = (uint32_t)n;

			if (n >= 0)
				mLo = 0xffffffff;
			else
				mLo = 1;
		} else if ((uint32_t)n == 0x80000000 && d == -1) {
			// Result is not representable in a 32bit
			// signed integer
			mHi = 0;
			mLo = 0x80000000;
		} else {
			mHi = (uint32_t)(n % d);
			mLo = (uint32_t)(n / d);
		}
	}

	void Cpu::opMflo(Instruction &instruction)
	{
		auto d = instruction.d();

		setReg(d, mLo);
	}

	void Cpu::opSrl(Instruction &instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = reg(t) >> i;

		setReg(d, v);
	}

	void Cpu::opSltiu(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = reg(s) < i;

		setReg(t, v);
	}

	// Divide Unsigned
	void Cpu::opDivu(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		auto n = reg(s);
		auto d = reg(t);

		if (d == 0)
		{
			// Division by zero, results are bogus
			mHi = n;
			mLo = 0xffffffff;
		} else {
			mHi = n % d;
			mLo = n / d;
		}
	}

	void Cpu::opMfhi(Instruction &instruction)
	{
		auto d = instruction.d();

		setReg(d, mHi);
	}

	void Cpu::opSlt(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		int32_t s_i = reg(s);
		int32_t t_i = reg(t);

		auto v = s_i < t_i;

		setReg(d, v);
	}

	void Cpu::opMtlo(Instruction &instruction)
	{
		auto s = instruction.s();

		mLo = reg(s);
	}

	void Cpu::opMthi(Instruction &instruction)
	{
		auto s = instruction.s();

		mHi = reg(s);
	}

	void Cpu::opLhu(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;

		// Address must be 16bit aligned
		if (addr % 2 == 0)
		{
			auto v = load16(addr);

			// Put the load in the delay slot
			mLoadRegIdx.val = t.val;
			mLoadReg = v;
		} else {
			exception(exception::LoadAddressError);
		}
	}

	void Cpu::opRfe(Instruction &instruction)
	{
		// There are other instructions with the same encoding but all
		// are virtual memory related and the Playstation doesn't
		// implement them. Still, let's make sure we're not running
		// buggy code.
		if ((instruction.mData & 0x3f) != 0b010000)
			panic("Invalid cop0 instruction: {}", instruction.mData);

		// Restore the pre-exception mode by shifting the Interrupt
		// Enable/User Mode stack back to its original position.
		auto mode = mSr & 0x3f;
		mSr &= !0x3f;
		mSr |= mode >> 2;
	}

	void Cpu::opSllv(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		// Shift amount is truncated to 5 bits
		auto v = reg(t) << (reg(s) & 0x1f);

		setReg(d, v);
	}

	void Cpu::opLh(Instruction &instruction)
	{
		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = reg(s) + i;

		// Cast as i16 to force sign extension
		int16_t v = (int16_t)load16(addr);

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opNor(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = ~(reg(s) | reg(t));

		setReg(d, v);
	}

	void Cpu::opSrav(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		// Shift amount is truncated to 5 bits
		auto v = ((int32_t)reg(t)) >> (reg(s) & 0x1f);

		setReg(d, v);
	}

	void Cpu::opSrlv(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		// Shift amount is truncated to 5 bits
		auto v = reg(t) >> (reg(s) & 0x1f);

		setReg(d, v);
	}

	void Cpu::opMultu(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		uint64_t a = reg(s);
		uint64_t b = reg(t);

		auto v = a * b;

		mHi = (uint32_t)(v >> 32);
		mLo = v;
	}

	void Cpu::opXor(Instruction &instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = reg(s) ^ reg(t);

		setReg(d, v);
	}

	void Cpu::opBreak(Instruction &instruction)
	{
		exception(exception::Break);
	}

	void Cpu::opMult(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		int64_t a = ((int32_t)reg(s));
		int64_t b = ((int32_t)reg(t));

		uint64_t v = (uint64_t)(a * b);

		mHi = (uint32_t)(v >> 32);
		mLo = (uint32_t)v;
	}

	void Cpu::opSub(Instruction &instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();
		auto d = instruction.d();

		uint32_t s_i = reg(s);
		uint32_t t_i = reg(t);

		uint32_t v = s_i - t_i;

		if (SubOverflow(s_i, t_i, v))
			exception(exception::Overflow);
		else
			setReg(d, v);
	}

	void Cpu::opXori(Instruction &instruction)
	{
		auto i = instruction.imm();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = reg(s) ^ i;

		setReg(t, v);
	}

	void Cpu::opCop1(Instruction &instruction)
	{
		exception(exception::CoprocessorError);
	}

	void Cpu::opCop2(Instruction &instruction)
	{
		panic("unhandled GTE instruction: {}", instruction.mData);
	}

	void Cpu::opCop3(Instruction &instruction)
	{
		exception(exception::CoprocessorError);
	}

	void Cpu::opLwl(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = (uint32_t)(reg(s) + i);

		// This instruction bypasses the load delay restriction: this
		// instruction will merge the new contents with the value
		// currently being loaded if need be.
		auto cur_v = mOutRegs[t.val];

		// Next we load the *aligned* word containing the first
		// addressed byte
		auto aligned_addr = addr & ~UINT32_C(3);
		auto aligned_word = load32(aligned_addr);

		// Depending on the address alignment we fetch the 1, 2, 3 or
		// 4 *most* significant bytes and put them in the target
		// register.
		uint32_t v;
		switch (addr & 3)
		{
		case 0:
			v = (cur_v & 0x00ffffff) | (aligned_word << 24);
			break;
		case 1:
			v = (cur_v & 0x0000ffff) | (aligned_word << 16);
			break;
		case 2:
			v = (cur_v & 0x000000ff) | (aligned_word << 8);
			break;
		case 3:
			v = (cur_v & 0x00000000) | (aligned_word << 0);
			break;
		default:
			panic("unreachable");
		};

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opLwr(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = (uint32_t)(reg(s) + i);

		// This instruction bypasses the load delay restriction: this
		// instruction will merge the new contents with the value
		// currently being loaded if need be.
		auto cur_v = mOutRegs[t.val];

		// Next we load the *aligned* word containing the first
		// addressed byte
		auto aligned_addr = addr & ~UINT32_C(3);
		auto aligned_word = load32(aligned_addr);

		// Depending on the address alignment we fetch the 1, 2, 3 or
		// 4 *least* significant bytes and put them in the target
		// register.
		uint32_t v;
		switch (addr & 3)
		{
		case 0:
			v = (cur_v & 0x00ffffff) | (aligned_word >> 24);
			break;
		case 1:
			v = (cur_v & 0x0000ffff) | (aligned_word >> 16);
			break;
		case 2:
			v = (cur_v & 0x000000ff) | (aligned_word >> 8);
			break;
		case 3:
			v = (cur_v & 0x00000000) | (aligned_word >> 0);
			break;
		default:
			panic("unreachable");
		};

		// Put the load in the delay slot
		mLoadRegIdx.val = t.val;
		mLoadReg = v;
	}

	void Cpu::opSwl(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = (uint32_t)(reg(s) + i);
		auto v = reg(t);

		auto aligned_addr = addr & ~UINT32_C(3);
		// Load the current value for the aligned word at the target
		// address
		auto cur_mem = load32(aligned_addr);

		uint32_t mem;
		switch (addr & 3)
		{
		case 0:
			mem = (cur_mem & 0x00ffffff) | (v >> 24);
			break;
		case 1:
			mem = (cur_mem & 0x0000ffff) | (v >> 16);
			break;
		case 2:
			mem = (cur_mem & 0x000000ff) | (v >> 8);
			break;
		case 3:
			mem = (cur_mem & 0x00000000) | (v >> 0);
			break;
		default:
			panic("unreachable");
		};

		store32(aligned_addr, mem);
	}

	void Cpu::opSwr(Instruction &instruction)
	{

		auto i = instruction.imm_se();
		auto t = instruction.t();
		auto s = instruction.s();

		uint32_t addr = (uint32_t)(reg(s) + i);
		auto v = reg(t);

		auto aligned_addr = addr & ~UINT32_C(3);
		// Load the current value for the aligned word at the target
		// address
		auto cur_mem = load32(aligned_addr);

		uint32_t mem;
		switch (addr & 3)
		{
		case 0:
			mem = (cur_mem & 0x00ffffff) | (v << 24);
			break;
		case 1:
			mem = (cur_mem & 0x0000ffff) | (v << 16);
			break;
		case 2:
			mem = (cur_mem & 0x000000ff) | (v << 8);
			break;
		case 3:
			mem = (cur_mem & 0x00000000) | (v << 0);
			break;
		default:
			panic("unreachable");
		};

		store32(aligned_addr, mem);
	}

	void Cpu::opLwc0(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opLwc1(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opLwc2(Instruction &instruction)
	{
		panic("unhandled GTE LWC: {}", instruction.mData);
	}

	void Cpu::opLwc3(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opSwc0(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opSwc1(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opSwc2(Instruction &instruction)
	{
		panic("unhandled GTE SWC: {}", instruction.mData);
	}

	void Cpu::opSwc3(Instruction &instruction)
	{
		// Not supported by this coprocessor
		exception(exception::CoprocessorError);
	}

	void Cpu::opIllegal(Instruction &instruction)
	{
		println("Illegal instruction {}!", instruction.mData);
		exception(exception::IllegalInstruction);
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
	RegisterIndex Instruction::t()
	{
		RegisterIndex ret((mData >> 16) & 0x1f);
		return ret;
	}

	// Return register index in bits [25:21]
	RegisterIndex Instruction::s()
	{
		RegisterIndex ret((mData >> 21) & 0x1f);
		return ret;
	}

	// Return register index in bits [15:11]
	RegisterIndex Instruction::d()
	{
		RegisterIndex ret((mData >> 11) & 0x1f);
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
