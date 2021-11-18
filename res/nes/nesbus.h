#ifndef _NESBUS_H_
#define _NESBUS_H_

#include "../sfot/defs.h"

// The bus for the nes
struct nesbus
{
public: // Variables 
	// 2KB of RAM for sfot
	u8 m_RAM[0x0800]{ 0 };
	// 8B for ppu
	u8 m_PPURegisters[0x0008]{ 0 };
	// 24B for apu/IO
	u8 m_APUIORegisters[0x0018]{ 0 };
	// ROM space (Max 0xBFE0)
	u8* m_ROM{ nullptr };

public: // Operator overloading
	u8& operator[](const u16& _index) { return ReadMapping(_index); }

private: // Internal methods
	u8& ReadMapping(const u16& _index);

public: // Instantiation
	/// @brief Initialize nesbus
	nesbus() { m_ROM = new u8[0xBFE0]{ 0 }; };
	// Destruction
	~nesbus() { delete m_ROM; };
};

#endif // !_NESBUS_H_
