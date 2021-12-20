#include "nesbus.h"

const u16 nesbus::PPUGetMirroring(const u16& _addr)
{
	u16 addr = _addr & 0x0FFF;
	if (m_Cart->GetNametableMirroring())
	{
		// Horizontal mirroring (vertical games)
		if (addr >= 0x0400 && addr < 0x0800) { addr -= 0x0400; }
		else if (addr >= 0x0C00) { addr -= 0x0800; }
	}
	else
	{
		// Vertical mirroring (horizontal games)
		if (addr >= 0x0800) { addr -= 0x0800; }
	}
	return addr;
}

const u16 nesbus::PALGetMirroring(const u16& _addr)
{
	u16 addr = _addr & 0x1F;
	switch (addr)
	{
	case 0x10:
	case 0x14:
	case 0x18:
	case 0x1C:
		addr -= 0x10;
		break;
	}
	return addr;
}

const u8 nesbus::CPUReadPPUReg(const u16 _addr)
{
	u8 data = 0;
	switch (_addr)
	{
	case 0x02: // PPUSTATUS
		data = (m_PPUReg->r_PPUSTATUS & 0x1F) | (m_PPUReg->r_VRAMABuffer & 0xE0);
		m_PPUReg->r_WriteLatch = 0; // Reset latch
		m_PPUReg->r_PPUSTATUS &= 0x7F; // Clear VBLANK flag
		break;
	case 0x04: // OAMDATA
		break;
	case 0x07: // PPUDATA
		data = m_PPUReg->r_VRAMABuffer; // Read previous address
		m_PPUReg->r_VRAMABuffer = PPURead(m_PPUReg->r_VRAMA);
		if (m_PPUReg->r_VRAMA >= 0x3F00) { data = m_PPUReg->r_VRAMABuffer; } // Use current if address is from pal ram
		m_PPUReg->r_VRAMA += (m_PPUReg->GetVRAMInc() ? 32 : 1);
		break;
	}
	return data;
}

void nesbus::CPUWritePPUReg(const u16 _addr, const u8& _data)
{
	switch (_addr)
	{
	case 0x00: // PPUCTRL
		m_PPUReg->r_PPUCTRL = _data;
		m_PPUReg->r_TRAMA = (m_PPUReg->r_TRAMA & 0x73FF) | (u16)((m_PPUReg->r_PPUCTRL & 0x03) << 10); // Set nametable x/y
		break;
	case 0x01: // PPUMASK
		m_PPUReg->r_PPUMASK = _data;
		break;
	case 0x03: // OAMADDR
		break;
	case 0x04: // OAMDATA
		break;
	case 0x05: // PPUSCROLL
		if (!m_PPUReg->r_WriteLatch)
		{
			m_PPUReg->r_FineX = _data & 0x07;
			m_PPUReg->r_TRAMA = (m_PPUReg->r_TRAMA & 0x7FE0) | (_data >> 3); // Set coarse X
		}
		else
		{
			m_PPUReg->r_TRAMA = (m_PPUReg->r_TRAMA & 0x0C1F) | (u16)((_data & 0x07) << 12) | (u16)((_data >> 3) << 5); // Set fine Y and coarse Y
		}
		m_PPUReg->r_WriteLatch = !m_PPUReg->r_WriteLatch; // Toggle write latch
		break;
	case 0x06: // PPUADDR
		m_PPUReg->r_TRAMA = (!m_PPUReg->r_WriteLatch ? (m_PPUReg->r_TRAMA & 0x00FF) | (u16)((_data & 0x3F) << 8) : (m_PPUReg->r_TRAMA & 0xFF00) | _data );
		if (m_PPUReg->r_WriteLatch) { m_PPUReg->r_VRAMA = m_PPUReg->r_TRAMA; } // Transfer TRAMA to VRAMA
		m_PPUReg->r_WriteLatch = !m_PPUReg->r_WriteLatch; // Toggle write latch
		break;
	case 0x07: // PPUDATA
		PPUWrite(m_PPUReg->r_VRAMA, _data);
		m_PPUReg->r_VRAMA += (m_PPUReg->GetVRAMInc() ? 32 : 1);
		break;
	}
}

const u8 nesbus::CPURead(const u16& _addr)
{
	if (_addr <= 0x1FFF) { return m_CPURAM[_addr & 0x07FF]; }
	if (_addr <= 0x3FFF) { return CPUReadPPUReg(_addr & 0x0007); }
	if (_addr <= 0x4017) { return 0; } // APU & IO
	if (_addr <= 0x401F) { return 0; } // Test functionality
	return m_Cart->CPURead(_addr);
}
void nesbus::CPUWrite(const u16& _addr, const u8& _data)
{
	if (_addr <= 0x1FFF) { m_CPURAM[_addr & 0x07FF] = _data; return; }
	if (_addr <= 0x3FFF) { CPUWritePPUReg(_addr & 0x0007, _data); return; }
	if (_addr <= 0x4017) { return; } // APU & IO
	if (_addr <= 0x401F) { return; } // Test functionality
	m_Cart->CPUWrite(_addr, _data);
}

const u8 nesbus::PPURead(const u16& _addr)
{
	u16 addr = _addr & 0x3FFF;
	if (addr <= 0x1FFF) { return m_Cart->PPURead(addr); }
	if (addr <= 0x3EFF)
	{ 
		u8 data = m_Cart->PPURead(addr);
		return (m_Cart->WasAccessed() ? data : m_PPURAM[PPUGetMirroring(addr)]);
	}
	return m_PALRAM[PALGetMirroring(addr)];
}
void nesbus::PPUWrite(const u16& _addr, const u8& _data)
{
	if (_addr <= 0x1FFF) { m_Cart->PPUWrite(_addr, _data); return; }
	if (_addr <= 0x3EFF)
	{
		m_Cart->PPUWrite(_addr, _data);
		if (!m_Cart->WasAccessed()) { m_PPURAM[PPUGetMirroring(_addr)] = _data; };
		return;
	}
	if (_addr <= 0x3FFF) { m_PALRAM[PALGetMirroring(_addr)] = _data; return; }
}