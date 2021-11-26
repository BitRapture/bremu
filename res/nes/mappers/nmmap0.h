#ifndef _NM_MAP_0_H_
#define _NM_MAP_0_H_

#include "nesmapper.h"

// NROM (iNes Mapper 0)
struct nmmap0 : public nesmapper
{
private: // Internal variables
	u32 m_Mirroring{ 0x3FFF };

public: // Method overriding
	bool CPURead(u16& _addr) override
	{
		if (_addr < 0x6000) { return false; }
		if (_addr < 0x8000) { _addr = m_PRGSize + (_addr - 0x6000); return true; } // RAM
		_addr = _addr & m_Mirroring; // 0x8000 - 0xFFFF
		return true;
	}
	bool PPURead(u16& _addr) override
	{
		return (_addr <= 0x1FFF);
	}
	bool CPUWrite(u16& _addr, const u8& _data) override
	{
		return CPURead(_addr);
	}
	bool PPUWrite(u16& _addr, const u8& _data) override
	{
		return (_addr <= 0x1FFF && m_CHRBanks == 0);
	}

public: // Instantiation
	nmmap0(u32& _PRGBanks, u32& _CHRBanks, u32& _PRGSize, u32& _CHRSize) : nesmapper(_PRGBanks, _CHRBanks, _PRGSize, _CHRSize) 
	{  
		if (m_PRGBanks > 1) { m_Mirroring = 0x7FFF; }
	}
};

#endif // !_NM_MAP_0_H_
