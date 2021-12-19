#include "nesppu.h"

void nesppu::AssemblePixel()
{

}

void nesppu::LoadBGShifters()
{
	r_PTHigh = (r_PTHigh & 0x00FF) | (u16)(r_PTHighLatch << 8);
	r_PTLow = (r_PTLow & 0x00FF) | (u16)(r_PTLowLatch << 8);

}

void nesppu::ShiftBGShifters()
{
	r_PTHigh >>= 1;
	r_PTLow >>= 1;
	r_PAHigh >>= 1;
	r_PALow >>= 1;
}

void nesppu::EmulateCycle(nesbus& _memory)
{
	// Pre render

	// Render 

	// Vblank

	// Put pixel onto screen buffer
	AssemblePixel();
	// Update cycles
	if (++e_Cycles > 340) { e_Cycles = 0; if (++e_Scanline > 260) { e_Scanline = -1; CreateFrame(); e_OddFrame = !e_OddFrame; } }
}

nesppu::nesppu()
{
	// Create screen
	m_Screen = new u32[256 * 240]{ 0 };
}

nesppu::~nesppu()
{
	delete[] m_Screen;
}