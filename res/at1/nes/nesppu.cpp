#include "nesppu.h"

void nesppu::DrawScreenDot(nesbus& _bus)
{
	u16 bitMux16 = 0x8000 >> r_REGISTERS.r_FINEX;

	u8 bgPixel = ((r_PTSR[0] & bitMux16) > 0) | (((r_PTSR[1] & bitMux16) > 0) << 1),
	bgPal = ((r_PASR[0] & bitMux16) > 0) | (((r_PASR[1] & bitMux16) > 0) << 1);

	u8 pixel = _bus.PPUReadMapping(0x3F00 + (bgPal << 2) + bgPixel) & 0x3F;

	u32 dot = e_Cycles - 1, y = e_Scanline - 1;
	if (dot > 255 || y > 239) { return; } // Overscan not rendered
	m_Screen[(y * 256) + dot] = e_Palette[pixel];
}

void nesppu::TileFetch(nesbus& _bus)
{
	u16 addr = 0;
	switch ((e_Cycles - 1) % 8)
	{
	case 0:
		LoadBGShifters();
		addr = 0x2000 | (r_REGISTERS.r_PPUADDR & 0x0FFF); // Nametable address
		l_NTB = _bus.PPUReadMapping(addr); // Get nametable byte
		break;
	case 2:
		addr = 0x23C0 | (r_REGISTERS.r_PPUADDR & 0x0C00) | ((r_REGISTERS.r_PPUADDR >> 4) & 0x38) | ((r_REGISTERS.r_PPUADDR >> 2) & 0x07); // Attribute address
		l_ATB = _bus.PPUReadMapping(addr); // Get attribute byte
		l_ATB >>= ((r_REGISTERS.r_PPUADDR & 0x40) ? 4 : 0) + ((r_REGISTERS.r_PPUADDR & 0x02) ? 2 : 0); // Coarse Y
		l_ATB &= 0x03;
		break;
	case 4:
		addr = (((r_REGISTERS.r_PPUCTRL >> 4) & 0x1) << 12) + ((u16)l_NTB << 4) + (r_REGISTERS.r_PPUADDR >> 12); // Pattern LSByte address
		l_PTL = _bus.PPUReadMapping(addr);
		break;
	case 6:
		addr = (((r_REGISTERS.r_PPUCTRL >> 4) & 0x1) << 12) + ((u16)l_NTB << 4) + ((r_REGISTERS.r_PPUADDR >> 12) + 8); // Pattern MSByte address 
		l_PTH = _bus.PPUReadMapping(addr);
		break;
	case 7:
		// Increment BG X Pointer (change nametable wrapping)
		if (r_REGISTERS.r_PPUMASK & 0x08)
		{
			r_REGISTERS.r_PPUADDR = ((r_REGISTERS.r_PPUADDR & 0x1F) == 0x1F ? (r_REGISTERS.r_PPUADDR & 0x7BE0) | ~(r_REGISTERS.r_PPUADDR & 0x400) : (r_REGISTERS.r_PPUADDR & 0x7FE0) | ((r_REGISTERS.r_PPUADDR & 0x1F) + 1));
		}
		break;
	}

}

void nesppu::LoadBGShifters()
{
	// Load pattern table shift registers with latches
	r_PTSR[0] = (r_PTSR[0] & 0xFF00) | l_PTL;
	r_PTSR[1] = (r_PTSR[1] & 0xFF00) | l_PTH;
	// Load pattern attribute shift registers with latches
	r_PASR[0] = (r_PASR[0] & 0xFF00) | ((l_ATB & 0x01) ? 0xFF : 0x00);
	r_PASR[1] = (r_PASR[1] & 0xFF00) | ((l_ATB & 0x02) ? 0xFF : 0x00);
}

void nesppu::UpdateShifters()
{
	if (r_REGISTERS.r_PPUMASK & 0x08)
	{
		r_PTSR[0] <<= 1;
		r_PTSR[1] <<= 1;
		r_PASR[0] <<= 1;
		r_PASR[1] <<= 1;
	}
}

void nesppu::RenderBackground(nesbus& _bus)
{
	if (e_Cycles > 1) { UpdateShifters(); } // First reload == Cycle 9
	TileFetch(_bus);
}

void nesppu::RenderSprites(nesbus& _bus)
{

}

void nesppu::CheckRenderTiming(nesbus& _bus)
{
	if (e_Cycles == 0) { /* Idle cycle */ return; }
	if (e_Cycles <= 256) { RenderBackground(_bus); return; }
	if (e_Cycles <= 320) { RenderSprites(_bus); return; }
	if (e_Cycles <= 336) { RenderBackground(_bus); return; }
	// Increment BG Y Pointer (change nametable wrapping)
	if (e_Cycles == 256 && (r_REGISTERS.r_PPUMASK & 0x08))
	{
		if ((r_REGISTERS.r_PPUADDR & 0x7000) != 0x7000) { r_REGISTERS.r_PPUADDR += 0x1000; }
		else
		{
			r_REGISTERS.r_PPUADDR &= 0xFFF;
			u32 coarseY = (r_REGISTERS.r_PPUADDR & 0x03E0) >> 5;
			switch (coarseY)
			{
			case 29:
				coarseY = 0;
				r_REGISTERS.r_PPUADDR ^= 0x0800;
				break;
			case 31:
				coarseY = 0;
				break;
			default:
				++coarseY;
			}
			r_REGISTERS.r_PPUADDR = (r_REGISTERS.r_PPUADDR & 0x7C1F) | (coarseY << 5);
		}
		return;
	}	
	// Reset BG X Pointer
	if (e_Cycles == 257)
	{
		LoadBGShifters();
		if (r_REGISTERS.r_PPUMASK & 0x08)
		{
			r_REGISTERS.r_PPUADDR = (r_REGISTERS.r_PPUADDR & 0x7BE0) | (r_REGISTERS.r_PPUTADDR & 0x41F);
		}
		return;
	}
	if (e_Cycles == 338 || e_Cycles == 340) 
	{ 
		// Useless reading of nametable data
		u16 addr = 0x2000 | (r_REGISTERS.r_PPUADDR & 0x0FFF); 
		l_NTB = _bus.PPUReadMapping(addr); 
		return; 
	}
}

void nesppu::CheckPrerenderTiming(nesbus& _bus)
{
	CheckRenderTiming(_bus); // Doesn't render, but makes the same memory accesses to fill up shifters
	if (e_Cycles == 1) { r_REGISTERS.r_PPUSTATUS &= 0x7F; return; } // Clear VBLANK
	// Reset Y pointer
	if (e_Cycles >= 280 && e_Cycles < 305 && (r_REGISTERS.r_PPUMASK & 0x08))
	{
		r_REGISTERS.r_PPUADDR = (r_REGISTERS.r_PPUADDR & 0x41F) | (r_REGISTERS.r_PPUTADDR & 0x7BE0); return;
	}
	if (e_OddFrame && e_Cycles == 339) { ++e_Cycles; } // Skip cycle on odd frame
}

void nesppu::CheckVBLANKTiming()
{
	if (e_Cycles == 1) 
	{ 
		r_REGISTERS.r_PPUSTATUS |= 0x80; // VBLANK period has begun
		if (r_REGISTERS.r_PPUCTRL & 0x80) { GenNMI(); } // If generate NMI is enabled, generate an NMI
	}
}

void nesppu::ScanlineTiming(nesbus& _bus)
{
	if (e_Scanline == 0) { CheckPrerenderTiming(_bus); return; }
	if (e_Scanline <= 240) { CheckRenderTiming(_bus); return; }
	if (e_Scanline == 241) { /* Idle scanline */ return; }
	if (e_Scanline == 242) { CheckVBLANKTiming(); return; }
}

bool nesppu::EmulateCycle(nesbus& _bus)
{
	ScanlineTiming(_bus);
	DrawScreenDot(_bus);
	// Progress cycles/scanline
	if (++e_Cycles > 340) { e_Cycles = 0; if (++e_Scanline > 261) { e_Scanline = 0; e_OddFrame = !e_OddFrame; } }
	// Return if frame has been rendered
	return (e_Cycles == 0 && e_Scanline == 0);
}

nesppu::nesppu()
{
	// Set nes resolution
	m_Screen = new u32[256 * 240];

	// Initialise default palette
	e_Palette[0] = 0x656565; e_Palette[16] = 0xaeaeae; e_Palette[32] = 0xfefeff;
	e_Palette[1] = 0x002d69; e_Palette[17] = 0x0f63b3; e_Palette[33] = 0x5db3ff; 
	e_Palette[2] = 0x131f7f; e_Palette[18] = 0x4051d0; e_Palette[34] = 0x8fa1ff; 
	e_Palette[3] = 0x3c137c; e_Palette[19] = 0x7841cc; e_Palette[35] = 0xc890ff;  
	e_Palette[4] = 0x600b62; e_Palette[20] = 0xa736a9; e_Palette[36] = 0xf785fa;
	e_Palette[5] = 0x730a37; e_Palette[21] = 0xc03470; e_Palette[37] = 0xff83c0;
	e_Palette[6] = 0x710f07; e_Palette[22] = 0xbd3c30; e_Palette[38] = 0xff8b7f;
	e_Palette[7] = 0x5a1a00; e_Palette[23] = 0x9f4a00; e_Palette[39] = 0xef9a49;
	e_Palette[8] = 0x342800; e_Palette[24] = 0x6d5c00; e_Palette[40] = 0xbdac2c;
	e_Palette[9] = 0x0b3400; e_Palette[25] = 0x366d00; e_Palette[41] = 0x85bc2f;
	e_Palette[10] = 0x003c00; e_Palette[26] = 0x077704; e_Palette[42] = 0x55c753;
	e_Palette[11] = 0x003d10; e_Palette[27] = 0x00793d; e_Palette[43] = 0x3cc98c;
	e_Palette[12] = 0x003840; e_Palette[28] = 0x00727d; e_Palette[44] = 0x3ec2cd;
	e_Palette[13] = 0x000000; e_Palette[29] = 0x000000; e_Palette[45] = 0x4e4e4e;
	e_Palette[14] = 0x000000; e_Palette[30] = 0x000000; e_Palette[46] = 0x000000;
	e_Palette[15] = 0x000000; e_Palette[31] = 0x000000; e_Palette[47] = 0x000000;
}