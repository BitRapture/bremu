#ifndef _PPU_H_
#define _PPU_H_

#include <functional>

#include "../defs.h"
#include "nesbus.h"
#include "nesppureg.h"

// The picture processing unit of the nes
struct nesppu
{
private: // Internal emulation variables
	// Generate NMI flag
	bool e_GenNMI{ false };

	// Current cycle of ppu (X)
	u32 e_Cycles{ 0 };
	// Current scanline of ppu (Y)
	s32 e_Scanline{ -1 };

	// If the current rendering frame is an odd frame
	bool e_OddFrame{ false };

private: // Internal Components
	// BG shifters
	// Pattern table shifters
	u16 r_PTHigh{ 0 }, r_PTLow{ 0 };
	// Palette attribute table shifters
	u16 r_PAHigh{ 0 }, r_PALow{ 0 };

	// Latches
	// Nametable latch
	u8 r_NTLatch{ 0 };
	// Attribute table latch
	u8 r_ATLatch{ 0 };
	// Pattern table latches
	u8 r_PTHighLatch{ 0 }, r_PTLowLatch{ 0 };

public: // Public variables
	// Full NES screen resolution
	u32* m_Screen{ nullptr };

	// Memory mapped registers
	nesppureg m_Reg;

public: // Getters
	const bool& GetGenNMI() { return e_GenNMI; }
	void ClearGenNMI() { e_GenNMI = false; }

private: // Internal methods
	// Create pixel from component data
	void AssemblePixel();

	void LoadBGShifters();

	void ShiftBGShifters();

public: // Public methods
	// Emulate one cycle of the ppu
	void EmulateCycle(nesbus& _memory);

	// Create a full screen frame
	std::function<void()> CreateFrame;

public: // Instantiation
	nesppu();
	~nesppu();
};

#endif // !_PPU_H_
