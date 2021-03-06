#include <gpu/gpu.hpp>
#include <gpu/opengl/renderer.hpp>

using gpu::opengl::renderer::Vertex;
using gpu::opengl::renderer::Position;
using gpu::opengl::renderer::Color;

namespace gpu {

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

CommandBuffer::CommandBuffer() : mLen(0)
{
	for (int i = 0; i < 12; i++)
		mBuffer[i] = 0;
}

CommandBuffer::~CommandBuffer()
{
}

void CommandBuffer::clear()
{
	mLen = 0;
}

void CommandBuffer::pushWord(uint32_t word)
{
	mBuffer[mLen] = word;
	mLen++;
}

uint32_t & CommandBuffer::operator[](uint32_t index)
{
	return mBuffer[index];
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
	mDmaDirection(DmaDirection::Off),
	mGp0WordsRemaining(0),
	mGp0CommandMethod(&Gpu::gp0Nop),
	mGp0Mode(Gp0Mode::Command)
{
	mRenderer.init();
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
	// XXX Temporary hack: if we don't emulate bit 31 correctly
	// setting `vres` to 1 locks the BIOS:
	//r |= ((uint32_t)mVres) << 19;
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
	println("GPUREAD");
	return 0;
}

void Gpu::gp0(uint32_t val)
{
	if (mGp0WordsRemaining == 0)
	{
		// We start a new GP0 command
		auto opcode = (val >> 24) & 0xff;

		switch (opcode)
		{
		case 0x00:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0Nop;
			break;
		case 0x01:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0ClearCache;
			break;
		case 0x28:
			mGp0WordsRemaining = 5;
			mGp0CommandMethod = &Gpu::gp0QuadMonoOpaque;
			break;
		case 0x2c:
			mGp0WordsRemaining = 9;
			mGp0CommandMethod = &Gpu::gp0QuadTextureBlendOpaque;
			break;
		case 0x30:
			mGp0WordsRemaining = 6;
			mGp0CommandMethod = &Gpu::gp0TriangleShadedOpaque;
			break;
		case 0x38:
			mGp0WordsRemaining = 8;
			mGp0CommandMethod = &Gpu::gp0QuadShadedOpaque;
			break;
		case 0xa0:
			mGp0WordsRemaining = 3;
			mGp0CommandMethod = &Gpu::gp0ImageLoad;
			break;
		case 0xc0:
			mGp0WordsRemaining = 3;
			mGp0CommandMethod = &Gpu::gp0ImageStore;
			break;
		case 0xe1:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0DrawMode;
			break;
		case 0xe2:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0TextureWindow;
			break;
		case 0xe3:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0DrawingAreaTopLeft;
			break;
		case 0xe4:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0DrawingAreaBottomRight;
			break;
		case 0xe5:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0DrawingOffset;
			break;
		case 0xe6:
			mGp0WordsRemaining = 1;
			mGp0CommandMethod = &Gpu::gp0MaskBitSetting;
			break;
		default:
			panic("Unhandled GP0 command {:08x}", val);
		}

		mGp0Command.clear();
	}

	mGp0WordsRemaining--;

	switch (mGp0Mode)
	{
	case Gp0Mode::Command:
		mGp0Command.pushWord(val);
		if (mGp0WordsRemaining == 0)
			// We have all the parameters, we can run the command
			((*this).*mGp0CommandMethod)();
		break;
	case Gp0Mode::ImageLoad:
		// XXX Should copy pixel data to VRAM
		if (mGp0WordsRemaining == 0)
			// Load done, switch back to command mode
			mGp0Mode = Gp0Mode::Command;
		break;
	}
}

void Gpu::gp0DrawMode()
{
	uint32_t val = mGp0Command[0];
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

void Gpu::gp0Nop()
{
	// NOP
}

void Gpu::gp0ClearCache()
{
	// Not implemented
}

void Gpu::gp0QuadMonoOpaque()
{
	// Only one color repeated 4 times
	Color color = Color::fromPacked(mGp0Command[0]);
	Vertex v1 = {Position::fromPacked(mGp0Command[1]), color};
	Vertex v2 = {Position::fromPacked(mGp0Command[2]), color};
	Vertex v3 = {Position::fromPacked(mGp0Command[3]), color};
	Vertex v4 = {Position::fromPacked(mGp0Command[4]), color};

	mRenderer.pushQuad(v1, v2, v3, v4);
}

void Gpu::gp0QuadTextureBlendOpaque()
{
	// XXX We don't support textures for now, use a solid red
	// color instead
	Color color = {1.0f, 0.0f, 0.0f};
	Vertex v1 = {Position::fromPacked(mGp0Command[1]), color};
	Vertex v2 = {Position::fromPacked(mGp0Command[3]), color};
	Vertex v3 = {Position::fromPacked(mGp0Command[5]), color};
	Vertex v4 = {Position::fromPacked(mGp0Command[7]), color};

	mRenderer.pushQuad(v1, v2, v3, v4);
}

void Gpu::gp0TriangleShadedOpaque()
{
	Vertex v1 = {Position::fromPacked(mGp0Command[1]), Color::fromPacked(mGp0Command[0])};
	Vertex v2 = {Position::fromPacked(mGp0Command[3]), Color::fromPacked(mGp0Command[2])};
	Vertex v3 = {Position::fromPacked(mGp0Command[5]), Color::fromPacked(mGp0Command[4])};

	mRenderer.pushTriangle(v1, v2, v3);
}

void Gpu::gp0QuadShadedOpaque()
{
	Vertex v1 = {Position::fromPacked(mGp0Command[1]), Color::fromPacked(mGp0Command[0])};
	Vertex v2 = {Position::fromPacked(mGp0Command[3]), Color::fromPacked(mGp0Command[2])};
	Vertex v3 = {Position::fromPacked(mGp0Command[5]), Color::fromPacked(mGp0Command[4])};
	Vertex v4 = {Position::fromPacked(mGp0Command[7]), Color::fromPacked(mGp0Command[6])};

	mRenderer.pushQuad(v1, v2, v3, v4);
}

void Gpu::gp0ImageLoad()
{
	// Parameter 2 contains the image resolution
	auto res = mGp0Command[2];

	auto width  = res & 0xffff;
	auto height = res >> 16;

	// Size of the image in 16bit pixels
	auto imgsize = width * height;

	// If we have an odd number of pixels we must round up since
	// we transfer 32bits at a time. There'll be 16bits of padding
	// in the last word.
	imgsize = (imgsize + 1) & ~1;

	// Store number of words expected for this image
	mGp0WordsRemaining = imgsize / 2;

	// Put the GP0 state machine in ImageLoad mode
	mGp0Mode = Gp0Mode::ImageLoad;
}

void Gpu::gp0ImageStore()
{
	// Parameter 2 contains the image resolution
	auto res = mGp0Command[2];

	auto width  = res & 0xffff;
	auto height = res >> 16;

	println("Unhandled image store: {}x{}", width, height);
}

void Gpu::gp0TextureWindow()
{
	uint32_t val = mGp0Command[0];
	mTextureWindowXMask = (val & 0x1f);
	mTextureWindowYMask = ((val >> 5) & 0x1f);
	mTextureWindowXOffset = ((val >> 10) & 0x1f);
	mTextureWindowYOffset = ((val >> 15) & 0x1f);
}

void Gpu::gp0DrawingAreaTopLeft()
{
	uint32_t val = mGp0Command[0];
	mDrawingAreaTop = ((val >> 10) & 0x3ff);
	mDrawingAreaLeft = (val & 0x3ff);
}

void Gpu::gp0DrawingAreaBottomRight()
{
	uint32_t val = mGp0Command[0];
	mDrawingAreaBottom = ((val >> 10) & 0x3ff);
	mDrawingAreaRight = (val & 0x3ff);
}

void Gpu::gp0DrawingOffset()
{
	uint32_t val = mGp0Command[0];
	uint16_t x = (val & 0x7ff);
	uint16_t y = ((val >> 11) & 0x7ff);

	// Values are 11bit two's complement signed values, we need to
	// shift the value to 16bits to force sign extension
	mDrawingXOffset = ((int16_t)(x << 5)) >> 5;
	mDrawingYOffset = ((int16_t)(y << 5)) >> 5;

	// XXX Temporary hack: force display when changing offset
	// since we don't have proper timings
	mRenderer.display();
	println("called display()");
}

void Gpu::gp0MaskBitSetting()
{
	uint32_t val = mGp0Command[0];
	mForceSetMaskBit = (val & 1) != 0;
	mPreserveMaskedPixels = (val & 2) != 0;
}

void Gpu::gp1(uint32_t val)
{
	auto opcode = (val >> 24) & 0xff;

	switch (opcode)
	{
	case 0x00:
		gp1Reset(val);
		break;
	case 0x01:
		gp1ResetCommandBuffer();
		break;
	case 0x02:
		gp1AcknowledgeIrq();
		break;
	case 0x03:
		gp1DisplayEnable(val);
		break;
	case 0x04:
		gp1DmaDirection(val);
		break;
	case 0x05:
		gp1DisplayVramStart(val);
		break;
	case 0x06:
		gp1DisplayHorizontalRange(val);
		break;
	case 0x07:
		gp1DisplayVerticalRange(val);
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

	gp1ResetCommandBuffer();
	gp1AcknowledgeIrq();

	// XXX should also invalidate GPU cache if we ever implement it
}

void Gpu::gp1ResetCommandBuffer()
{
	mGp0Command.clear();
	mGp0WordsRemaining = 0;
	mGp0Mode = Gp0Mode::Command;
	// XXX should also clear the command FIFO when we implement it
}

void Gpu::gp1AcknowledgeIrq()
{
	mInterrupt = false;
}

void Gpu::gp1DisplayEnable(uint32_t val)
{
	mDisplayDisabled = ((val & 1) != 0);
}

void Gpu::gp1DmaDirection(uint32_t val)
{
	switch (val & 3)
	{
	case 0:
		mDmaDirection = DmaDirection::Off;
		break;
	case 1:
		mDmaDirection = DmaDirection::Fifo;
		break;
	case 2:
		mDmaDirection = DmaDirection::CpuToGp0;
		break;
	case 3:
		mDmaDirection = DmaDirection::VRamToCpu;
		break;
	};
}

void Gpu::gp1DisplayVramStart(uint32_t val)
{
	mDisplayVramXStart = (val & 0x3fe);
	mDisplayVramYStart = ((val >> 10) & 0x1ff);
}

void Gpu::gp1DisplayHorizontalRange(uint32_t val)
{
	mDisplayHorizStart = (val & 0xfff);
	mDisplayHorizEnd   = ((val >> 12) & 0xfff);
}

void Gpu::gp1DisplayVerticalRange(uint32_t val)
{
	mDisplayLineStart = (val & 0x3ff);
	mDisplayLineEnd   = ((val >> 10) & 0x3ff);
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

} // namespace gpu