#ifndef _NESBUS_H_
#define _NESBUS_H_

#include "../defs.h"
#include "nescart.h"
#include "nesppureg.h"

// The bus of the nes 
struct nesbus
{
private: // Memory
	u8 m_CPURAM[0x0800]{ 0 };
	u8 m_PPURAM[0x0800]{ 0 };
	u8 m_PALRAM[0x0020]{ 0 };

public: // Connections
	nescart* m_Cart{ nullptr };
	nesppureg* m_PPUReg{ nullptr };

private: // Internal methods
	const u16 PPUGetMirroring(const u16& _addr);
	const u16 PALGetMirroring(const u16& _addr);

	const u8 CPUReadPPUReg(const u16 _addr);
	void CPUWritePPUReg(const u16 _addr, const u8& _data);

public: // Public methods
	const u8 CPURead(const u16& _addr);
	void CPUWrite(const u16& _addr, const u8& _data);

	const u8 PPURead(const u16& _addr);
	void PPUWrite(const u16& _addr, const u8& _data);
};

#endif // !_NESBUS_H_
