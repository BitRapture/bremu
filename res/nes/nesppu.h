#ifndef _PPU_H_
#define _PPU_H_

#include <functional>

#include "../defs.h"
#include "nesbus.h"
#include "nesppureg.h"

// The picture processing unit of the nes
struct nesppu
{
private: // Internal emulation variables
	// Generate NMI flag
	bool e_GenNMI{ false };

public: // Public variables
	// Full NES screen resolution
	u8* m_Screen{ nullptr };

	// Memory mapped registers
	nesppureg m_Reg;

public: // Getters
	const bool& GetGenNMI() { return e_GenNMI; }
	void ClearGenNMI() { e_GenNMI = false; }

public: // Public methods
	void EmulateCycle(nesbus& _memory);

	// Create a full screen frame
	std::function<void()> CreateFrame;

public: // Instantiation
	nesppu();
};

#endif // !_PPU_H_
