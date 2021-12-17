#ifndef _NESPPUREG_H_
#define _NESPPUREG_H_

// Encapsulation of memory mapped registers for the PPU and CPU
struct nesppureg
{
public: // Registers
	// Loopy's registers
	u16 r_VRAMA{ 0 }; // 15 bits
	u16 r_TRAMA{ 0 }; // 15 bits
	u8 r_FineX{ 0 }; // 3 bits
	bool r_WriteLatch{ 0 };

	// VRAMA data buffer
	u8 r_VRAMABuffer{ 0 };

	// Full registers
	u8 r_PPUCTRL{ 0 };
	u8 r_PPUMASK{ 0 };
	u8 r_PPUSTATUS{ 0 };

public: // Getters & Setters
	const bool GetVBLANK() { return (r_PPUCTRL & 0x80); }
	const bool GetVRAMInc() { return (r_PPUCTRL & 0x04); }
	
	const bool ShowBackground() { return (r_PPUMASK & 0x08); }
	const bool ShowSprites() { return (r_PPUMASK & 0x10); }
};

#endif // !_NESPPUREG_H_
