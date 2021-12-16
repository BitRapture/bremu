#include "nesbus.h"

void nesbus::PPURegisterWrite(const u16& _index, u8& _data)
{
	switch (_index & 0x0007)
	{
	case 0x0000: // PPUCTRL
		m_PPUREG->r_PPUCTRL = _data;
		m_PPUREG->r_PPUTADDR = (m_PPUREG->r_PPUTADDR & 0x73ff) | ((u16)(_data & 0x03) << 10); // Nametable data
		break;
	case 0x0001: // PPUMASK
		m_PPUREG->r_PPUMASK = _data;
		break;
	//case 0x0002: // PPUSTATUS
	case 0x0003: // OAMADDR
		m_PPUREG->r_OAMADDR = _data;
		break;
	case 0x0004: // OAMDATA
		m_PPUREG->r_OAMDATA = _data;
		++(m_PPUREG->r_OAMADDR);
		break;
	case 0x0005: // PPUSCROLL
		if (m_PPUREG->l_PPUADDR)
		{
			m_PPUREG->r_PPUTADDR = (m_PPUREG->r_PPUTADDR & 0x7FE0) | (_data >> 3);
			m_PPUREG->r_FINEX = _data & 0x07;
		}
		else
		{
			m_PPUREG->r_PPUTADDR = (m_PPUREG->r_PPUTADDR & 0x0C1F) | ((_data & 0x07) << 12) | ((_data >> 3) << 5);
		}
		m_PPUREG->l_PPUADDR = !m_PPUREG->l_PPUADDR; // 2 cycle write
		break;
	case 0x0006: // PPUADDR
		m_PPUREG->r_PPUTADDR = (m_PPUREG->l_PPUADDR ? (m_PPUREG->r_PPUTADDR & 0xFF00) | _data : (m_PPUREG->r_PPUTADDR & 0x00FF) | ((_data & 0x3F) << 8));
		if (m_PPUREG->l_PPUADDR) { m_PPUREG->r_PPUADDR = m_PPUREG->r_PPUTADDR; }
		m_PPUREG->l_PPUADDR = !m_PPUREG->l_PPUADDR; // 2 cycle write
		break;
	case 0x0007: // PPUDATA
		PPUWriteMapping(m_PPUREG->r_PPUADDR, _data);
		m_PPUREG->r_PPUADDR += (m_PPUREG->r_PPUCTRL & 0x04 ? 32 : 1); // Choose increment
		m_PPUREG->r_PPUADDR &= 0x3FFF;
		break;
	}
}

const u8 nesbus::PPURegisterRead(const u16& _index)
{
	u8 data = 0;
	switch (_index & 0x0007)
	{
	//case 0x0000: // PPUCTRL
	//case 0x0001: // PPUMASK
	case 0x0002: // PPUSTATUS
		data = (m_PPUREG->r_PPUSTATUS & 0xE0) | (m_PPUREG->r_PPUBUFFER & 0x1F);
		m_PPUREG->l_PPUADDR = 0; // Clear address latch
		m_PPUREG->r_PPUSTATUS &= 0x7F; // Clear vertical blank flag
		break;
	// case 0x0003: // OAMADDR
	case 0x0004: // OAMDATA
		data = m_PPUREG->r_OAMDATA;
		break;
	// case 0x0005: // PPUSCROLL
	// case 0x0006: // PPUADDR
	case 0x0007: // PPUDATA
		data = m_PPUREG->r_PPUBUFFER;
		m_PPUREG->r_PPUBUFFER = PPUReadMapping(m_PPUREG->r_PPUADDR);
		if (m_PPUREG->r_PPUADDR >= 0x3F00) { data = m_PPUREG->r_PPUBUFFER; }
		m_PPUREG->r_PPUADDR += (m_PPUREG->r_PPUCTRL & 0x04 ? 32 : 1); // Choose increment
		m_PPUREG->r_PPUADDR &= 0x3FFF;
		break;
	}
	return data;
}

u8& nesbus::PPUMapRAM(const u16& _index)
{
	u16 addr = 0, iaddr = _index & 0x0FFF;
	if (PPUGetNMirror())
	{
		if (iaddr >= 0x0400 && iaddr <= 0x07FF || iaddr >= 0x0C00 && iaddr <= 0x0FFF) { addr = 0x003FF; }
	}
	else
	{
		if (iaddr >= 0x0800 && iaddr <= 0x0BFF || iaddr >= 0x0C00 && iaddr <= 0x0FFF) { addr = 0x003FF; }
	}
	return m_PPURAM[addr + (iaddr & 0x003FF)];
}

// Above should probably be moved into the PPUREG struct
//		-> then move this to another file

const u8& nesbus::CPUReadMapping(const u16& _index)
{
	if (_index <= 0x1FFF) { return m_SFOTRAM[_index & 0x07FF]; }
	if (_index <= 0x3FFF) { return PPURegisterRead(_index); }
	if (_index <= 0x4017) { return m_APUIORegisters[_index - 0x4000]; }
	if (_index <= 0x401F) { return m_ROM->m_Null; } // Implement CPU Test (0x4018 to 0x401F)
	return m_ROM->CPURead(_index); // 0x4020 - 0xFFFF
}

const u8& nesbus::PPUReadMapping(const u16& _index)
{
	if (_index <= 0x1FFF) { return m_ROM->PPURead(_index); } // Pattern tables
	if (_index <= 0x3EFF) // Nametables 
	{ 
		u8* loc = &m_ROM->PPURead(_index);
		return (loc != &m_ROM->m_Null ? *loc : PPUMapRAM(_index)); // Check if cartridge has mapped nametables
	}
	return m_PALRAM[_index & 0x1F];  // Palette indexes
}

void nesbus::CPUWriteMapping(const u16& _index, u8 _data)
{
	if (_index <= 0x1FFF) { m_SFOTRAM[_index & 0x07FF] = _data; return;  }
	if (_index <= 0x3FFF) { PPURegisterWrite(_index, _data); return; }
	if (_index <= 0x4017) { m_APUIORegisters[_index - 0x4000] = _data; return; }
	if (_index <= 0x401F) { return; } 
	m_ROM->CPUWrite(_index, _data);
}

void nesbus::PPUWriteMapping(const u16& _index, u8 _data)
{
	if (_index <= 0x1FFF) { m_ROM->PPUWrite(_index, _data); return; } // Pattern tables (CHR RAM)
	if (_index <= 0x3EFF) // Nametables 
	{
		if (!m_ROM->PPUWrite(_index, _data)) { PPUMapRAM(_index) = _data; } return;
	}
	m_PALRAM[_index & 0x1F] = _data; // Palette indexes
}

nesbus::nesbus()
{

}