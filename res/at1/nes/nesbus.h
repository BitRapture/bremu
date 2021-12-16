#ifndef _NESBUS_H_
#define _NESBUS_H_

#include "../sfot/defs.h"
#include "nescart.h"

// PPU Registers to be loaded into bus
struct nesppureg
{
	u8 r_PPUCTRL{ 0 };
	u8 r_PPUMASK{ 0 };
	u8 r_PPUSTATUS{ 0 };
	u8 r_OAMADDR{ 0 };
	u8 r_OAMDATA{ 0 };
	u8 r_FINEX{ 0 }; // PPUSCROLL
	u16 r_PPUADDR{ 0 }; bool l_PPUADDR{ false }; // VRAM address (15 bits) & latch
	u16 r_PPUTADDR{ 0 }; // Temporary VRAM address (15 bits)
	u8 r_PPUBUFFER{ 0 }; // Buffer for PPU Reads through CPU
};

// The bus for the nes
struct nesbus
{
public: // Variables 
	// 2KB of RAM for sfot
	u8 m_SFOTRAM[0x0800]{ 0 };
	// 2KB of RAM for ppu
	u8 m_PPURAM[0x0800]{ 0 };
	// 32B for palette RAM indices
	u8 m_PALRAM[0x0020]{ 0 }; 
	// PPU Registers
	nesppureg* m_PPUREG{ nullptr };
	// 24B for apu/IO
	u8 m_APUIORegisters[0x0018]{ 0 };
	// ROM space
	nescart* m_ROM{ nullptr };

private: // Internal methods
	void PPURegisterWrite(const u16& _index, u8& _data);
	const u8 PPURegisterRead(const u16& _index);
	u8& PPUMapRAM(const u16& _index);

public: // Public methods
	const u8& CPUReadMapping(const u16& _index);
	const u8& PPUReadMapping(const u16& _index);
	void CPUWriteMapping(const u16& _index, u8 _data);
	void PPUWriteMapping(const u16& _index, u8 _data);

	/// @brief Return nametable mirroring config for ppu
	/// @return 0 for horizontal, 1 for vertical mirroring
	bool PPUGetNMirror() { return m_ROM->m_MetaData[6] & 1; }

public: // Instantiation
	/// @brief Initialize nesbus
	nesbus();
};

#endif // !_NESBUS_H_
