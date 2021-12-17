#ifndef _NMMAP0_H_
#define _NMMAP0_H_

#include "../../defs.h"
#include "nesmapper.h"

// Nes mapper 0: nrom
struct nmmap0 : public nesmapper
{
private:
	u32 m_Mirroring{ 0x3FFF };

public: // Public methods
	const u8 CPURead(const u16& _addr) override
	{
		m_Access = false;
		if (_addr < 0x6000) { return 0; } // No access
		m_Access = true;
		if (m_PRGRAMSize != 0 && _addr <= 0x7FFF) { return m_PRGRAM[_addr & (m_PRGRAMSize - 1)]; } // RAM access
		return m_PRGROM[_addr & m_Mirroring]; // PRG ROM access
	}
	void CPUWrite(const u16& _addr, const u8& _data) override
	{
		m_Access = false;
		if (_addr < 0x6000) { return; } // No access
		m_Access = true;
		if (m_PRGRAMSize != 0 && _addr <= 0x7FFF) { m_PRGRAM[_addr & (m_PRGRAMSize - 1)] = _data; return; } // RAM access
		m_PRGROM[_addr & m_Mirroring] = _data; // PRG ROM access
	}

	const u8 PPURead(const u16& _addr) override
	{
		m_Access = true;
		if (_addr <= 0x1FFF) { return m_CHRROM[_addr]; } // CHR ROM access (pass through)
		m_Access = false;
		return 0;
	}
	void PPUWrite(const u16& _addr, const u8& _data) override
	{
		m_Access = true;
		if (_addr <= 0x1FFF) { m_CHRROM[_addr] = _data; return; } // CHR ROM access (pass through)
		m_Access = false;
	}

public: // Instantiation
	nmmap0()
	{
		if ((m_PRGROMSize / 0x4000) >= 1) { m_Mirroring = 0x7FFF; }
	}
};

#endif // !_NMMAP0_H_
