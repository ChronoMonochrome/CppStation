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
		mTextureWindowXMask(0),
		mTextureWindowYMask(0),
		mTextureWindowXOffset(0),
		mTextureWindowYOffset(0),
		mDithering(false),
		mDrawToDisplay(false),
		mForceSetMaskBit(false),
		mPreserveMaskedPixels(false),
		mDrawingAreaLeft(0),
		mDrawingAreaTop(0),
		mDrawingAreaRight(0),
		mDrawingAreaBottom(0),
		mDrawingXOffset(0),
		mDrawingYOffset(0),
		mField(Field::Top),
		mTextureDisable(false),
		mHres(HorizontalRes::fromFields(0, 0)),
		mVres(VerticalRes::Y240Lines),
		mVmode(VMode::Ntsc),
		mDisplayDepth(DisplayDepth::D15Bits),
		mInterlaced(false),
		mDisplayDisabled(true),
		mDisplayVramXStart(0),
		mDisplayVramYStart(0),
		mDisplayHorizStart(0x200),
		mDisplayHorizEnd(0xc00),
		mDisplayLineStart(0x10),
		mDisplayLineEnd(0x100),
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

	uint32_t Gpu::read()
	{
		// Not implemented for now...
		return 0;
	}

	void Gpu::gp0(uint32_t val)
	{
		auto opcode = (val >> 24) & 0xff;

		switch (opcode)
		{
		case 0x00:
			break; // NOP
		case 0xe1:
			gp0DrawMode(val);
			break;
		default:
			panic("Unhandled GP0 command {:08x}", val);
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

	void Gpu::gp1(uint32_t val)
	{
		auto opcode = (val >> 24) & 0xff;

		switch (opcode)
		{
		case 0x00:
			gp1Reset(val);
			break;
		case 0x08:
			gp1DisplayMode(val);
			break;
		default:
			panic("Unhandled GP1 command {:08x}", val);
		}
	}

	void Gpu::gp1Reset(uint32_t val)
	{
		(void)val;
		mInterrupt = false;

		mPageBaseX = 0;
		mPageBaseY = 0;
		mSemiTransparency = 0;
		mTextureDepth = TextureDepth::T4Bit;
		mTextureWindowXMask = 0;
		mTextureWindowYMask = 0;
		mTextureWindowXOffset = 0;
		mTextureWindowYOffset = 0;
		mDithering = false;
		mDrawToDisplay = false;
		mTextureDisable = false;
		mRectangleTextureXFlip = false;
		mRectangleTextureYFlip = false;
		mDrawingAreaLeft = 0;
		mDrawingAreaTop = 0;
		mDrawingAreaRight = 0;
		mDrawingAreaBottom = 0;
		mDrawingXOffset = 0;
		mDrawingYOffset = 0;
		mForceSetMaskBit = false;
		mPreserveMaskedPixels = false;

		mDmaDirection = DmaDirection::Off;

		mDisplayDisabled = true;
		mDisplayVramXStart = 0;
		mDisplayVramYStart = 0;
		mHres = HorizontalRes::fromFields(0, 0);
		mVres = VerticalRes::Y240Lines;

		// XXX does PAL hardware reset to this config as well?
		mVmode = VMode::Ntsc;
		mInterlaced = true;
		mDisplayHorizStart = 0x200;
		mDisplayHorizEnd = 0xc00;
		mDisplayLineStart = 0x10;
		mDisplayLineEnd = 0x100;
		mDisplayDepth = DisplayDepth::D15Bits;

		// XXX should also clear the command FIFO when we implement it
		// XXX should also invalidate GPU cache if we ever implement it
    }

	void Gpu::gp1DisplayMode(uint32_t val)
	{
		uint8_t hr1 = (val & 3);
		uint8_t hr2 = ((val >> 6) & 1);

		mHres = HorizontalRes::fromFields(hr1, hr2);

		mVres = (val & 0x4 != 0) ? VerticalRes::Y480Lines : VerticalRes::Y240Lines;

		mVmode = (val & 0x8 != 0) ? VMode::Pal : VMode::Ntsc;

		mDisplayDepth = (val & 0x10 != 0) ? DisplayDepth::D15Bits : DisplayDepth::D24Bits;

		mInterlaced = val & 0x20 != 0;

		if ((val & 0x80) != 0)
			panic("Unsupported display mode {:08x}", val);
    }
}