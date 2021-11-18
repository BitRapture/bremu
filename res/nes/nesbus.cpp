#include "nesbus.h"

u8& nesbus::ReadMapping(const u16& _index)
{
	if (_index <= 0x1FFF) { return m_RAM[_index & 0x07FF]; }
	if (_index <= 0x3FFF) { return m_PPURegisters[_index & 0x0007]; }
	if (_index <= 0x4017) { return m_APUIORegisters[_index - 0x4000]; }
	if (_index <= 0x401F) { return m_RAM[0]; } // Implement CPU Test (0x4018 to 0x401F)
	return m_ROM[_index - 0x4020]; 
}