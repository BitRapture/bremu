#ifndef _NESPPU_H_
#define _NESPPU_H_

#include <functional>
#include <SDL.h> // Since PPU is the rendering component, it will require a rendering library

#include "nesbus.h"

// The PPU (Picture Processing Unit) for the nes emulator
struct nesppu
{
public: // Rendering variables
	u32* m_Screen{ nullptr };
	u32 m_PalettePitch{ 4 * 256 };

private: // Defs
	// Memory mapped registers addresses
	enum struct r_REG { PPUCTRL = 0x2000, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUADDR, PPUDATA, OAMDMA = 0x4014 };

private: // PPU internals 
	// Shift registers
	u16 r_PTSR[2]{ 0 };	// Pattern table shift registers
	u16 r_PASR[2]{ 0 }; // Pattern attribute shift registers

	// Internal latches
	u8 l_NTB{ 0 }; // Nametable byte latch
	u8 l_ATB{ 0 }; // Attribute table byte latch
	u8 l_PTL{ 0 }, l_PTH{ 0 }; // Pattern table LOW & HIGH Latches
	
	// Object attribute memory (might not be needed)
	//u8 r_OAM[0x0100]{ 0 };

public: // Registers
	// Memory mapped registers for bus interaction
	nesppureg r_REGISTERS;

private: // Internal emulation variables
	// The current cycle of the ppu
	u32 e_Cycles{ 0 };
	// Current scanline
	u32 e_Scanline{ 0 };

	// True when rendering an odd frame
	bool e_OddFrame{ false };

public: // External emulation variables
	// Palette for indexing
	u32 e_Palette[64];

private: // Internal methods
	void DrawScreenDot(nesbus& _bus);

	void TileFetch(nesbus& _bus);

	void LoadBGShifters();
	void UpdateShifters();

	/// @brief Render the background tiles
	void RenderBackground(nesbus& _bus);
	/// @brief Render the sprites
	void RenderSprites(nesbus& _bus);

	/// @brief Emulates the cycle timing for the prerendering scanlines
	void CheckPrerenderTiming(nesbus& _bus);
	/// @brief Emulates the cycle timing for the visible scanlines
	void CheckRenderTiming(nesbus& _bus);
	/// @brief Emulates the cycle timing for the vblank scanlines
	void CheckVBLANKTiming();
	/// @brief Emulates the timing for scanlines
	void ScanlineTiming(nesbus& _bus);

public: // Emulation methods
	/// @brief Emulate a PPU cycle
	/// @param _bus Connection to memory mapped registers & chr rom
	/// @return Frame completed
	bool EmulateCycle(nesbus& _bus);

	/// @brief Generate an NMI for cpu
	std::function<void(void)> GenNMI;

	const u32 GetDot() { return ((e_Scanline * 256) + e_Cycles); }
	const u32 GetCycles() { return e_Cycles; }
	const u32 GetScanline() { return e_Scanline; }

public: // Instantiation
	nesppu();
	~nesppu() { delete[] m_Screen; }
};

#endif // !_NESPPU_H_
