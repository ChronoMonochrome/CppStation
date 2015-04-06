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
		// Mirror textured rectangles along the x axis
		bool mRectangleTextureXFlip;
		// Mirror textured rectangles along the y axis
		bool mRectangleTextureYFlip;
		// Semi-transparency. Not entirely sure how to handle that value
		// yet, it seems to describe how to blend the source and
		// destination colors.
		uint8_t mSemiTransparency;
		// Texture page color depth
		TextureDepth mTextureDepth;
		// Texture window x mask (8 pixel steps)
		uint8_t mTextureWindowXMask;
		// Texture window y mask (8 pixel steps)
		uint8_t mTextureWindowYMask;
		// Texture window x offset (8 pixel steps)
		uint8_t mTextureWindowXOffset;
		// Texture window y offset (8 pixel steps)
		uint8_t mTextureWindowYOffset;
		// Enable dithering from 24 to 15bits RGB
		bool mDithering;
		// Allow drawing to the display area
		bool mDrawToDisplay;
		// Force "mask" bit of the pixel to 1 when writing to VRAM
		// (otherwise don't modify it)
		bool mForceSetMaskBit;
		// Don't draw to pixels which have the "mask" bit set
		bool mPreserveMaskedPixels;
		// Left-most column of drawing area
		uint16_t mDrawingAreaLeft;
		// Top-most line of drawing area
		uint16_t mDrawingAreaTop;
		// Right-most column of drawing area
		uint16_t mDrawingAreaRight;
		// Bottom-most line of drawing area
		uint16_t mDrawingAreaBottom;
		// Horizontal drawing offset applied to all vertex
		int16_t mDrawingXOffset;
		// Vertical drawing offset applied to all vertex
		int16_t mDrawingYOffset;
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
		// First column of the display area in VRAM
		uint16_t mDisplayVramXStart;
		// First line of the display area in VRAM
		uint16_t mDisplayVramYStart;
		// Display output horizontal start relative to HSYNC
		uint16_t mDisplayHorizStart;
		// Display output horizontal end relative to HSYNC
		uint16_t mDisplayHorizEnd;
		// Display output first line relative to VSYNC
		uint16_t mDisplayLineStart;
		// Display output last line relative to VSYNC
		uint16_t mDisplayLineEnd;
		// True when the interrupt is active
		bool mInterrupt;
		// DMA request direction
		DmaDirection mDmaDirection;

		// Retreive value of the status register
		uint32_t status();

		// Retreive value of the "read" register
		uint32_t read();

		// Handle writes to the GP0 command register
		void gp0(uint32_t val);

		// GP0(0xE1): Draw Mode
		void gp0DrawMode(uint32_t val);

		// GP0(0xE2): Set Texture Window
		void gp0TextureWindow(uint32_t val);

		// GP0(0xE3): Set Drawing Area top left
		void gp0DrawingAreaTopLeft(uint32_t val);

		// GP0(0xE4): Set Drawing Area bottom right
		void gp0DrawingAreaBottomRight(uint32_t val);

		// GP0(0xE5): Set Drawing Offset
		void gp0DrawingOffset(uint32_t val);

		// Handle writes to the GP1 command register
		void gp1(uint32_t val);

		// GP1(0x00): soft reset
		void gp1Reset(uint32_t val);

		// GP1(0x04): DMA Direction
		void gp1DmaDirection(uint32_t val);

		// GP1(0x08): Display Mode
		void gp1DisplayMode(uint32_t val);

		// Linkage to the communications bus
		bus::Bus *mBus = nullptr;
		// Link GPU to a communications bus
		void connectBus(bus::Bus *n) { mBus = n; }
	};
}