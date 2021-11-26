#ifndef _NESBUS_H_
#define _NESBUS_H_

#include "../sfot/defs.h"
#include "nescart.h"

// The bus for the nes
struct nesbus
{
public: // Variables 
	// 2KB of RAM for sfot
	u8 m_SFOTRAM[0x0800]{ 0 };
	// 2KB of RAM for ppu
	u8 m_PPURAM[0x0800]{ 0 };
	// 8B for ppu
	u8 m_PPURegisters[0x0008]{ 0 };
	// 24B for apu/IO
	u8 m_APUIORegisters[0x0018]{ 0 };
	// ROM space
	nescart* m_ROM{ nullptr };

public: // Public methods
	const u8& CPUReadMapping(const u16& _index);
	const u8& PPUReadMapping(const u16& _index);
	void CPUWriteMapping(const u16& _index, u8 _data);
	void PPUWriteMapping(const u16& _index, u8 _data);

public: // Instantiation
	/// @brief Initialize nesbus
	nesbus(nescart& _cartDock) : m_ROM{ &_cartDock } { }
	nesbus() { }
};

#endif // !_NESBUS_H_
