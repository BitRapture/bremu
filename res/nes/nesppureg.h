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
	u8 r_OAMDATA{ 0 };
	u8 r_OAMADDR{ 0 };

	/// Direct memory access registers
	u8 r_PPUDMA{ 0 };
	u8 r_DMAccess{ 0 }; // Counter for accesses

public: // Getters & Setters
	bool GetVBLANK() { return ((r_PPUCTRL & 0x80) > 0); }
	bool GetVRAMInc() { return ((r_PPUCTRL & 0x04) > 0); }
	bool GetPatternTable() { return ((r_PPUCTRL & 0x10) > 0); }

	u8 GetFineY() { return (r_VRAMA >> 12); }
	u8 GetCoarseX() { return (r_VRAMA & 0x001F); }
	u8 GetCoarseY() { return ((r_VRAMA & 0x03E0) >> 5); }

	bool ShowBackground() { return ((r_PPUMASK & 0x08) > 0); }
	bool ShowSprites() { return ((r_PPUMASK & 0x10) > 0); }
	bool RenderingEnabled() { return (ShowBackground() || ShowSprites()); }
};

#endif // !_NESPPUREG_H_
