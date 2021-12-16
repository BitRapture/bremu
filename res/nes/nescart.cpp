#include "nescart.h"

void nescart::LoadCartridge(const char* _path)
{
	// Unload previous cart data
	delete[] m_PRGROM; m_PRGROM = nullptr;
	delete[] m_CHRROM; m_CHRROM = nullptr;
	m_MetaData[0] = 0;
	m_Loaded = false;

	// Load and read metadata from file
	std::ifstream cartSource(_path, std::ios::ate | std::ios::binary); if (!cartSource.is_open()) { return; }
	if (cartSource.tellg() < 17) { cartSource.close(); return; }
	cartSource.seekg(std::ios::beg);
	cartSource.read((s8*)m_MetaData, 0x10);

	// Check for iNes header
	if (!(m_MetaData[0] == 'N' && m_MetaData[1] == 'E' && m_MetaData[2] == 'S' && m_MetaData[3] == 0x1A)) { cartSource.close();  return; }
	// Check for iNes 2.0 support
	bool iNes2 = ((m_MetaData[7] & 0xC) == 8);

	// Get PRG size
	u32 iNes2PRG = m_MetaData[9] & 0xF;
	m_PRGROMSize = (iNes2 ? (iNes2PRG == 0xF ? (1 << ((m_MetaData[4] & 0xFC) >> 2)) * (((m_MetaData[4] & 0x3) * 2) + 1) : (iNes2PRG << 8) | m_MetaData[4]) : m_MetaData[4] * 16384);
	// Get CHR size
	u32 iNes2CHR = (m_MetaData[9] & 0xF0) >> 4;
	m_CHRROMSize = (iNes2 ? (iNes2CHR == 0xF ? (1 << ((m_MetaData[5] & 0xFC) >> 2)) * (((m_MetaData[5] & 0x3) * 2) + 1) : (iNes2CHR << 8) | m_MetaData[5]) : m_MetaData[5] * 8192);
	if (m_CHRROMSize == 0) { m_CHRROMSize = (iNes2 ? (m_MetaData[11] & 0xF ? 64 << (m_MetaData[11] & 0xF) : 64 << (m_MetaData[11] >> 4)) : 8192); }

	// Assemble mapper type
	m_MapperType = (iNes2 ? (m_MetaData[8] & 0xF) << 8 : 0) | (m_MetaData[7] & 0xF0) | (m_MetaData[6] >> 4);
	if (!CreateMapper()) { cartSource.close(); return; }

	// Skip trainer (todo: add modified carts)
	if (m_MetaData[6] & 0x4) { cartSource.seekg(std::ios::cur + 512); }

	// Check if cartridge has internal PRG RAM (0x6000 - 0x7FFF)
	m_PRGRAMSize = (m_MetaData[6] & 0x2 ? 8192 : 0); // Allocate 8KB of memory

	// Allocate PRG ROM
	m_PRGROM = new u8[m_PRGROMSize + m_PRGRAMSize]; // Combine PRG ROM and PRG RAM as cpu uses both of these
	cartSource.read((s8*)m_PRGROM, m_PRGROMSize);
	// Allocate CHR ROM
	m_CHRROM = new u8[m_CHRROMSize];
	cartSource.read((s8*)m_CHRROM, m_CHRROMSize);

	// Cart data successfully loaded
	m_Loaded = true;
	cartSource.close();
}

bool nescart::CreateMapper()
{
	// Destroy previous mapper
	delete m_Mapper; m_Mapper = nullptr;
	u32 prgBanks = m_PRGROMSize / 16384,
		chrBanks = m_CHRROMSize / 8192;

	// Choose mapper
	switch (m_MapperType)
	{
	case 0: // NROM
		m_Mapper = new nmmap0(prgBanks, chrBanks, m_PRGROMSize, m_CHRROMSize);
		break;
	case 1:	// MMC1
	case 2: // UxROM

	default: 
		return false;
	}

	return true;
}