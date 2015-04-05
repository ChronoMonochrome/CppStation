#include <gpu/gpu.hpp>


namespace gpu
{
	HorizontalRes::HorizontalRes(uint8_t hr) :
		mHr(hr)
	{
	}

	HorizontalRes::~HorizontalRes()
	{
	}

	uint8_t HorizontalRes::fromFields(uint8_t hr1, uint8_t hr2)
	{
		return (hr2 & 1) | ((hr1 & 3) << 1);
	}

	uint32_t HorizontalRes::infoStatus()
	{
		return mHr << 16;
	}

	Gpu::Gpu() :
		mPageBaseX(0),
		mPageBaseY(0),
		mRectangleTextureXFlip(false),
		mRectangleTextureYFlip(false),
		mSemiTransparency(0),
		mTextureDepth(TextureDepth::T4Bit),
		mDithering(false),
		mDrawToDisplay(false),
		mForceSetMaskBit(false),
		mPreserveMaskedPixels(false),
		mField(Field::Top),
		mTextureDisable(false),
		mHres(HorizontalRes::fromFields(0, 0)),
		mVres(VerticalRes::Y240Lines),
		mVmode(VMode::Ntsc),
		mDisplayDepth(DisplayDepth::D15Bits),
		mInterlaced(false),
		mDisplayDisabled(true),
		mInterrupt(false),
		mDmaDirection(DmaDirection::Off)
	{
	}

	Gpu::~Gpu()
	{
	}

	uint32_t Gpu::status()
	{
		uint32_t r = 0;

		r |= mPageBaseX << 0;
		r |= mPageBaseY << 4;
		r |= mSemiTransparency << 5;
		r |= ((uint32_t)mTextureDepth) << 7;
		r |= mDithering << 9;
		r |= mDrawToDisplay << 10;
		r |= mForceSetMaskBit << 11;
		r |= mPreserveMaskedPixels << 12;
		r |= ((uint32_t)mField) << 13;
		// Bit 14: not supported
		r |= mTextureDisable << 15;
		r |= mHres.infoStatus();
		r |= ((uint32_t)mVres) << 19;
		r |= ((uint32_t)mVmode) << 20;
		r |= ((uint32_t)mDisplayDepth) << 21;
		r |= mInterlaced << 22;
		r |= mDisplayDisabled << 23;
		r |= mInterrupt << 24;

		// For now we pretend that the GPU is always ready:
		// Ready to receive command
		r |= 1 << 26;
		// Ready to send VRAM to CPU
		r |= 1 << 27;
		// Ready to receive DMA block
		r |= 1 << 28;

		r |= ((uint32_t)mDmaDirection) << 29;

		// Bit 31 should change depending on the currently drawn line
		// (whether it's even, odd or in the vblack apparently). Let's
		// not bother with it for now.
		r |= 0 << 31;

		// Not sure about that, I'm guessing that it's the signal
		// checked by the DMA in when sending data in Request
		// synchronization mode. For now I blindly follow the Nocash
		// spec.
		uint8_t dmaRequest;
		switch (mDmaDirection)
		{
		// Always 0
		case DmaDirection::Off:
			dmaRequest = 0;
			break;
		// Should be 0 if FIFO is full, 1 otherwise
		case DmaDirection::Fifo:
			dmaRequest = 1;
			break;
		// Should be the same as status bit 28
		case DmaDirection::CpuToGp0:
			dmaRequest = (r >> 28) & 1;
			break;
		// Should be the same as status bit 27
		case DmaDirection::VRamToCpu:
			dmaRequest = (r >> 27) & 1;
			break;
		default:
			panic("%s: Unknown DMA direction: %d", __func__, (uint32_t)mDmaDirection);
		}

		r |= dmaRequest << 25;

		return r;
	}

	void Gpu::gp0(uint32_t val)
	{
		auto opcode = (val >> 24) & 0xff;

		switch (opcode)
		{
		case 0xe1:
			gp0DrawMode(val);
			break;
		default:
			panic("Unhandled GP0 opcode {:02x} ({:08x})", opcode, val);
		}
	}

	void Gpu::gp0DrawMode(uint32_t val)
	{
		mPageBaseX = val & 0xf;
		mPageBaseY = (val >> 4) & 1;
		mSemiTransparency = (val >> 5) & 3;

		uint32_t tmpVal = (val >> 7) & 3;

		switch (tmpVal)
		{
		case 0:
			mTextureDepth = TextureDepth::T4Bit;
			break;
		case 1:
			mTextureDepth = TextureDepth::T8Bit;
			break;
		case 2:
			mTextureDepth = TextureDepth::T15Bit;
			break;
		default:
			panic("Unhandled texture depth {}", tmpVal);
		}

		mDithering = ((val >> 9) & 1) != 0;
		mDrawToDisplay = ((val >> 10) & 1) != 0;
		mTextureDisable = ((val >> 11) & 1) != 0;
		mRectangleTextureXFlip = ((val >> 12) & 1) != 0;
		mRectangleTextureYFlip = ((val >> 13) & 1) != 0;
	}
}