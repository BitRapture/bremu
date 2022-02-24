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

public: // Public emulation variables
	// The palette used (default PAL palette)
	u32 e_Palette[0x40]{
	   0x00808080, 0x00003DA6, 0x000012B0, 0x00440096, 0x00A1005E, 0x00C70028, 0x00BA0600, 0x008C1700, 0x005C2F00, 0x00104500, 0x00054A00, 0x0000472E, 0x00004166, 0x00000000, 0x00050505, 0x00050505,
	   0x00C7C7C7, 0x000077FF, 0x002155FF, 0x008237FA, 0x00EB2FB5, 0x00FF2950, 0x00FF2200, 0x00D63200, 0x00C46200, 0x00358000, 0x00058F00, 0x00008A55, 0x000099CC, 0x00212121, 0x00090909, 0x00090909,
	   0x00FFFFFF, 0x000FD7FF, 0x0069A2FF, 0x00D480FF, 0x00FF45F3, 0x00FF618B, 0x00FF8833, 0x00FF9C12, 0x00FABC20, 0x009FE30E, 0x002BF035, 0x000CF0A4, 0x0005FBFF, 0x005E5E5E, 0x000D0D0D, 0x000D0D0D,
	   0x00FFFFFF, 0x00A6FCFF, 0x00B3ECFF, 0x00DAABEB, 0x00FFA8F9, 0x00FFABB3, 0x00FFD2B0, 0x00FFEFA6, 0x00FFF79C, 0x00D7E895, 0x00A6EDAF, 0x00A2F2DA, 0x0099FFFC, 0x00DDDDDD, 0x00111111, 0x00111111
	};

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
	void AssemblePixel(nesbus& _memory);

	void LoadBGShifters();

	void ShiftBGShifters();

	void IncrementX();
	void IncrementY();
	void TransferX();
	void TransferY();

public: // Public methods
	// Emulate one cycle of the ppu
	void EmulateCycle(nesbus& _memory);

	// Reset the ppu
	void Reset();

	// Create a full screen frame
	std::function<void()> CreateFrame;

public: // Instantiation
	nesppu();
	~nesppu();
};

#endif // !_PPU_H_
