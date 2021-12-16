#ifndef _NESMAPPER_H_
#define _NESMAPPER_H_

// Abstract mapper class for nes
struct nesmapper
{
public: // Public members
	// Banks of memory
	u32 m_PRGBanks{ 0 };
	u32 m_CHRBanks{ 0 };

	// Memory sizes
	u32 m_PRGSize{ 0 };
	u32 m_CHRSize{ 0 };

public: // Pure virtual methods
	virtual bool CPURead(u16& _addr) = 0;
	virtual bool PPURead(u16& _addr) = 0;
	virtual bool CPUWrite(u16& _addr, const u8& _data) = 0; // Might not need the bool, just checks if the mapper has written value internally such as mapper registers
	virtual bool PPUWrite(u16& _addr, const u8& _data) = 0;

public: // Instantiation
	nesmapper(u32& _PRGBanks, u32& _CHRBanks, u32& _PRGSize, u32& _CHRSize) : m_PRGBanks{ _PRGBanks }, m_CHRBanks{ _CHRBanks }, m_PRGSize{ _PRGSize }, m_CHRSize{ _CHRSize } { }
};

#endif // !_NESMAPPER_H_
