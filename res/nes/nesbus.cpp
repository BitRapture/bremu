#include "nesbus.h"

const u8& nesbus::CPUReadMapping(const u16& _index)
{
	if (_index <= 0x1FFF) { return m_SFOTRAM[_index & 0x07FF]; }
	if (_index <= 0x3FFF) { return m_PPURegisters[_index & 0x0007]; }
	if (_index <= 0x4017) { return m_APUIORegisters[_index - 0x4000]; }
	if (_index <= 0x401F) { return m_SFOTRAM[0]; } // Implement CPU Test (0x4018 to 0x401F)
	return m_ROM->CPURead(_index); // 0x4020 - 0xFFFF
}

const u8& nesbus::PPUReadMapping(const u16& _index)
{
	if (_index <= 0x1FFF) { return m_ROM->PPURead(_index); }
}

void nesbus::CPUWriteMapping(const u16& _index, u8 _data)
{
	if (_index <= 0x1FFF) { m_SFOTRAM[_index & 0x07FF] = _data; return;  }
	if (_index <= 0x3FFF) { m_PPURegisters[_index & 0x0007] = _data; return; }
	if (_index <= 0x4017) { m_APUIORegisters[_index - 0x4000] = _data; return; }
	if (_index <= 0x401F) { return; } 
	m_ROM->CPUWrite(_index, _data);
}

void nesbus::PPUWriteMapping(const u16& _index, u8 _data)
{

}