#include "nesppu.h"

void nesppu::AssemblePixel(nesbus& _memory)
{
	if (m_Reg.ShowBackground())
	{
		if (e_Cycles < 255 && e_Scanline >= 0 && e_Scanline < 240)
		{
			u16 bitMux = 0x8000 >> m_Reg.r_FineX;
			u8 bgPixel = ((r_PTLow & bitMux) > 0) | (((r_PTHigh & bitMux) > 0) << 1);
			u8 bgPalette = ((r_PALow & bitMux) > 0) | (((r_PAHigh & bitMux) > 0) << 1);

			m_Screen[(e_Scanline * 256) + e_Cycles] = e_Palette[_memory.PPURead(0x3F00 + (bgPalette << 2) + bgPixel) & 0x3F];
		}
	}
}

void nesppu::LoadBGShifters()
{
	r_PTHigh = (r_PTHigh & 0xFF00) | r_PTHighLatch;
	r_PTLow = (r_PTLow & 0xFF00) | r_PTLowLatch;
	r_PAHigh = (r_PAHigh & 0xFF00) | ((r_ATLatch & 0x02) ? 0xFF : 0x00);
	r_PALow = (r_PALow & 0xFF00) | ((r_ATLatch & 0x01) ? 0xFF : 0x00);
}

void nesppu::ShiftBGShifters()
{
	if (m_Reg.ShowBackground())
	{
		r_PTHigh <<= 1;
		r_PTLow <<= 1;
		r_PAHigh <<= 1;
		r_PALow <<= 1;
	}
}

void nesppu::IncrementX()
{
	// Increment scroll X, wrap around to next nametable
	if (m_Reg.RenderingEnabled())
	{
		m_Reg.r_VRAMA = (m_Reg.GetCoarseX() == 0x1F ? (m_Reg.r_VRAMA & 0x7FE0) ^ 0x0400  : m_Reg.r_VRAMA + 1);
	}
}

void nesppu::IncrementY()
{
	// Increment scroll Y, wrap around to next nametable
	if (m_Reg.RenderingEnabled())
	{
		if (m_Reg.GetFineY() < 7)
		{
			m_Reg.r_VRAMA += 0x1000; // Increment fine Y
		}
		else
		{
			m_Reg.r_VRAMA &= 0xFFF; // Clear fine Y
			u8 coarseY = m_Reg.GetCoarseY();
			switch (coarseY)
			{
			case 29: // Switch vertical nametable
				coarseY = 0;
				m_Reg.r_VRAMA ^= 0x0800;
				break;
			case 31: // Clear coarse Y
				coarseY = 0;
				break;
			default: // Increment coarse Y
				++coarseY;
			}
			m_Reg.r_VRAMA = (m_Reg.r_VRAMA & 0x7C1F) | (coarseY << 5);
		}
	}
}

void nesppu::TransferX()
{
	// Transfer temporary vram scroll X address to pointer
	if (m_Reg.RenderingEnabled())
	{
		// Transfer nametable X and coarse X to VRAMA
		m_Reg.r_VRAMA = (m_Reg.r_VRAMA & 0x7BE0) | (m_Reg.r_TRAMA & 0x041F);
	}
}

void nesppu::TransferY()
{
	// Transfer temporary vram scroll Y address to pointer
	if (m_Reg.RenderingEnabled())
	{
		// Transfer fine Y, nametable Y, and coarse Y to VRAMA
		m_Reg.r_VRAMA = (m_Reg.r_VRAMA & 0x041F) | (m_Reg.r_TRAMA & 0x7BE0);
	}
}

void nesppu::EmulateCycle(nesbus& _memory)
{
	if (e_Scanline < 240) // Rendering
	{
		// Fetch data
		if (e_Cycles >= 2 && e_Cycles <= 257 || e_Cycles >= 321 && e_Cycles <= 337) // Debug dont do anything for sprite rendering (return to 336 afterwards)
		{
			// Shifters first shift on cycle 2
			ShiftBGShifters();

			// Tile fetching
			switch ((e_Cycles - 1) % 8)
			{
			case 0:
				// Nametable fetch
				LoadBGShifters();
				r_NTLatch = _memory.PPURead(0x2000 | (m_Reg.r_VRAMA & 0x0FFF));
				break;
			case 2:
				// Attribute table fetch
				r_ATLatch = _memory.PPURead(0x23C0 | (m_Reg.r_VRAMA & 0x0C00) | ((m_Reg.r_VRAMA >> 4) & 0x38) | ((m_Reg.r_VRAMA >> 2) & 0x07));
				r_ATLatch >>= (m_Reg.GetCoarseY() & 0x02 ? 4 : 0) + (m_Reg.GetCoarseX() & 0x02 ? 2 : 0);
				r_ATLatch &= 0x03;
				break;
			case 4:
				// Pattern table low fetch
				r_PTLowLatch = _memory.PPURead(((u16)m_Reg.GetPatternTable() << 12) + ((u16)r_NTLatch << 4) + m_Reg.GetFineY());
				break;
			case 6:
				// Pattern table high fetch (+ 8 for next plane)
				r_PTHighLatch = _memory.PPURead(((u16)m_Reg.GetPatternTable() << 12) + ((u16)r_NTLatch << 4) + m_Reg.GetFineY() + 0x08);
				break;
			case 7:
				IncrementX();
				break;
			}

			// Incrementing and reloading scroll register
			switch (e_Cycles)
			{
			case 256:
				// End of visible rendering scanline
				IncrementY();
				break;
			case 257:
				// Reset X position
				LoadBGShifters();
				TransferX();
				break;
			}
		}
		else if (e_Cycles == 338 || e_Cycles == 340)
		{
			// Garbage nametable fetches
			r_NTLatch = _memory.PPURead(0x2000 | (m_Reg.r_VRAMA & 0x0FFF));
		}

		// Pre-render scanline
		if (e_Scanline == -1) 
		{ 
			// Clear VBLANK status flag
			if (e_Cycles == 1) { m_Reg.r_PPUSTATUS &= 0x7F; }
			// Reload vertical scroll
			if (e_Cycles >= 280 && e_Cycles <= 304 && m_Reg.RenderingEnabled()) { TransferY(); }
			// Odd frame skip
			if (e_Cycles == 339 && e_OddFrame) { ++e_Cycles; }
		}
	}
	else if (e_Scanline == 241) // VBLANK
	{
		// Set VBLANK
		if (e_Cycles == 1)
		{ 
			// VBLANK status flag set
			m_Reg.r_PPUSTATUS |= 0x80; 
			// Generate NMI
			e_GenNMI = m_Reg.GetVBLANK();
		}
	}

	// Put pixel onto screen buffer
	AssemblePixel(_memory);
	// Update cycles
	if (++e_Cycles > 340) { e_Cycles = 0; if (++e_Scanline > 260) { e_Scanline = -1; CreateFrame(); e_OddFrame = !e_OddFrame; } }
}

void nesppu::Reset()
{
	e_Cycles = 0; e_Scanline = -1;
	e_OddFrame = false;
	m_Reg = nesppureg();
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