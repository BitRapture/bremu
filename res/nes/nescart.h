#ifndef _NESCART_H_
#define _NESCART_H_

#include <fstream>
#include "../defs.h"

#include "mappers/nmmap0.h"

// The cartridge emulation (where game data is stored)
struct nescart
{
private: // Internal variables
	// Loaded flag, if cartridge has successfully loaded data
	bool m_Loaded{ false };

	// Mapper
	nesmapper* m_Mapper{ nullptr };
	u32 m_MapperType{ 0 };

public: // Public variables
	// iNes metadata
	u8 m_MetaData[0x10]{ 0 };

private: // Internal methods
	bool CreateMapper();

public: // Getters
	// Is the cartridge data loaded
	const bool& IsLoaded() { return m_Loaded; }
	// Was the cartridge successfully accessed
	const bool& WasAccessed() { return m_Mapper->m_Access; }

	// MetaData getters
	// Get mapper type
	const u32& GetMapperType() { return m_MapperType; }
	const bool GetNametableMirroring() { return (m_MetaData[6] & 0x01); }

public: // Public methods
	void LoadCartridge(const char* _path);

	const u8 CPURead(const u16& _addr) { return m_Mapper->CPURead(_addr); };
	void CPUWrite(const u16& _addr, const u8& _data) { m_Mapper->CPUWrite(_addr, _data); };

	const u8 PPURead(const u16& _addr) { return m_Mapper->PPURead(_addr); };
	void PPUWrite(const u16& _addr, const u8& _data) { m_Mapper->PPUWrite(_addr, _data); };

public: // Instantiation
	~nescart() { delete m_Mapper; }
};

#endif // !_NESCART_H_
