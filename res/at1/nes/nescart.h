#ifndef _NESCART_H_
#define _NESCART_H_

#include <fstream>
#include <string>

#include "../sfot/defs.h"
#include "mappers/nmmap0.h" // Swap out for one header include file later

// Cartridge ROM manager for the nes
struct nescart
{
public: // Variables
	// Safety measure in case reads return 0
	u8 m_Null{ 0 }; 

	// Flag; loaded cart contents into ROM space
	bool m_Loaded{ false };

	// iNes header metadata
	u8 m_MetaData[0x10]{ 0 };

	// CHR ROM & PRG ROM
	u8* m_PRGROM{ nullptr };
	u8* m_CHRROM{ nullptr };

	// CHR & PRG ROM Size
	u32 m_PRGROMSize{ 0 };
	u32 m_CHRROMSize{ 0 };

	// Cartridge PRG RAM present
	u32 m_PRGRAMSize{ 0 };

	// Mapper type
	u32 m_MapperType{ 0 };
	// Mapper chip
	nesmapper* m_Mapper{ nullptr };

public: // Public methods
	/// @brief Load cartridge from file to ROM space
	/// @param _path Cartridge file location
	/// @param _mem The nes bus
	void LoadCartridge(const char* _path);

	const u8& CPURead(u16 _addr) { return (m_Mapper->CPURead(_addr) ? m_PRGROM[_addr] : m_Null); };
	u8& PPURead(u16 _addr) { return (m_Mapper->PPURead(_addr) ? m_CHRROM[_addr] : m_Null); };
	void CPUWrite(u16 _addr, const u8& _data) { if (m_Mapper->CPUWrite(_addr, _data)) { m_PRGROM[_addr] = _data; }; }
	bool PPUWrite(u16 _addr, const u8& _data) { if (m_Mapper->PPUWrite(_addr, _data)) { m_CHRROM[_addr] = _data; return true; }; return false; }

private: // Internal methods
	/// @brief Instantiate the correct mapper for cart
	/// @return Creation success
	bool CreateMapper();

public: // Instantiation

	// Destructor
	~nescart() { delete[] m_CHRROM; delete[] m_PRGROM; }
};

#endif // !_NESCART_H_
