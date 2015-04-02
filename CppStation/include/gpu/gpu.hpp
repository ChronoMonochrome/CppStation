#pragma once

#include "helpers.hpp"

namespace bus {
	class Bus;
}

namespace gpu {
	// Depth of the pixel values in a texture page
	enum class TextureDepth
	{
		// 4 bits per pixel
		T4Bit = 0,
		// 8 bits per pixel
		T8Bit = 1,
		// 15 bits per pixel
		T15Bit = 2,
	};

	// Interlaced output splits each frame in two fields
	enum class Field
	{
		// Top field (odd lines).
		Top = 1,
		// Bottom field (even lines)
		Bottom = 0,
	};

	// Video output horizontal resolution
	class HorizontalRes
	{
	public:
		uint8_t mHr;
		HorizontalRes(uint8_t hr);
		~HorizontalRes();
		// Create a new HorizontalRes instance from the 2 bit field `hr1`
		// and the one bit field `hr2`
		static uint8_t fromFields(uint8_t hr1, uint8_t hr2);
		// Retreive value of bits [18:16] of the status register
		uint32_t infoStatus();
	};

	enum class VerticalRes {
		// 240 lines
		Y240Lines = 0,
		// 480 lines (only available for interlaced output)
		Y480Lines = 1,
	};

	enum class VMode {
		// NTSC: 480i60H
		Ntsc = 0,
		// PAL: 576i50Hz
		Pal  = 1,
	};

	enum class DisplayDepth {
		// 15 bits per pixel
		D15Bits = 0,
		// 24 bits per pixel
		D24Bits = 1,
	};

	enum class DmaDirection {
		Off = 0,
		Fifo = 1,
		CpuToGp0 = 2,
		VRamToCpu = 3,
	};

	class Gpu
	{
	public:
		Gpu();
		~Gpu();

		// Texture page base X coordinate (4 bits, 64 byte increment)
		uint8_t mPageBaseX;
		// Texture page base Y coordinate (1bit, 256 line increment)
		uint8_t mPageBaseY;
		// Semi-transparency. Not entirely sure how to handle that value
		// yet, it seems to describe how to blend the source and
		// destination colors.
		uint8_t mSemiTransparency;
		// Texture page color depth
		TextureDepth mTextureDepth;
		// Enable dithering from 24 to 15bits RGB
		bool mDithering;
		// Allow drawing to the display area
		bool mDrawToDisplay;
		// Force "mask" bit of the pixel to 1 when writing to VRAM
		// (otherwise don't modify it)
		bool mForceSetMaskBit;
		// Don't draw to pixels which have the "mask" bit set
		bool mPreserveMaskedPixels;
		// Currently displayed field. For progressive output this is
		// always Top.
		Field mField;
		// When true all textures are disabled
		bool mTextureDisable;
		// Video output horizontal resolution
		HorizontalRes mHres;
		// Video output vertical resolution
		VerticalRes mVres;
		// Video mode
		VMode mVmode;
		// Display depth. The GPU itself always draws 15bit RGB, 24bit
		// output must use external assets (pre-rendered textures, MDEC,
		// etc...)
		DisplayDepth mDisplayDepth;
		// Output interlaced video signal instead of progressive
		bool mInterlaced;
		// Disable the display
		bool mDisplayDisabled;
		// True when the interrupt is active
		bool mInterrupt;
		// DMA request direction
		DmaDirection mDmaDirection;

		// Retreive value of the status register
		uint32_t status();

		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link GPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}