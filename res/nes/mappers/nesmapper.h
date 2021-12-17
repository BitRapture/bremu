#ifndef _NESMAPPER_H_
#define _NESMAPPER_H_

#include "../../defs.h"

// Virtual abstract base class for all mappers
struct nesmapper
{
public: // Public variables
	u8* m_PRGROM{ nullptr }; u32 m_PRGROMSize{ 0 };
	u8* m_PRGRAM{ nullptr }; u32 m_PRGRAMSize{ 0 };
	u8* m_CHRROM{ nullptr }; u32 m_CHRROMSize{ 0 };

	// Access flag, if cart contents was able to be accessed
	bool m_Access{ false };

public: // Public methods
	virtual const u8 CPURead(const u16& _addr) = 0;
	virtual void CPUWrite(const u16& _addr, const u8& _data) = 0;

	virtual const u8 PPURead(const u16& _addr) = 0;
	virtual void PPUWrite(const u16& _addr, const u8& _data) = 0;
};

#endif // !_NESMAPPER_H_
