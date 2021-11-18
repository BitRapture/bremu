#include "sfot.h"

#include <iostream>

// Macro: set specific bit 
#define BIT_SET(_u, _n, _r) _u = (_u & ~(1 << _n)) | ((_r) << _n)
/*
	to-do:
		Add specifc status flag macros
			e.g: NEGATIVE_SET, ZERO_SET
	Make enums specify u8 type, remove type casting
*/

u16 sfot::AM_Abs(nesbus& _mem)
{
	// Get address (little-endian) from next two bytes
	u16 addr = _mem[r_PC]; ++r_PC;
	addr |= (_mem[r_PC] << 8); ++r_PC;
	return addr;
}
u16 sfot::AM_AbsX(nesbus& _mem)
{
	// Get address (little-endian) from next two bytes, offset by X
	u16 addr = _mem[r_PC]; ++r_PC;
	addr |= (_mem[r_PC] << 8); ++r_PC;
	addr += r_X;
	return addr;
}
u16 sfot::AM_AbsY(nesbus& _mem)
{
	// Get address (little-endian) from next two bytes, offset by Y
	u16 addr = _mem[r_PC]; ++r_PC;
	addr |= (_mem[r_PC] << 8); ++r_PC;
	addr += r_Y;
	return addr;
}
u16 sfot::AM_Ind(nesbus& _mem)
{
	// Get address indirectly from next two bytes
	u16 addr = _mem[r_PC]; ++r_PC;
	addr |= (_mem[r_PC] << 8); ++r_PC;
	addr = _mem[addr]; 
	return addr;
}
u16 sfot::AM_XInd(nesbus& _mem)
{
	// Get address from the zero-page + r_X with wrap around
	u8 zpg = (u8)(_mem[r_PC] + r_X); ++r_PC;
	return (u16)(_mem[zpg] | (_mem[(u8)(zpg + 1)] << 8));
}
u16 sfot::AM_IndY(nesbus& _mem)
{
	// Get address from the _mem(zero-page) + r_Y
	u16 zpg = _mem[r_PC]; ++r_PC;
	return (u16)(_mem[zpg] | (_mem[zpg + 1] << 8) + r_Y);
}
u16 sfot::AM_Rel(nesbus& _mem)
{
	// For branches, next byte is a signed offset for r_PC
	u16 addr = r_PC; 
	addr += (s8)(_mem[r_PC]); ++r_PC;
	return addr;
}
u16 sfot::AM_Zpg(nesbus& _mem)
{
	// Zero-page addressing
	return _mem[r_PC++];
}
u16 sfot::AM_ZpgX(nesbus& _mem)
{
	// Zero-page indirect addressing r_X
	return (u8)(_mem[r_PC++] + r_X);
}
u16 sfot::AM_ZpgY(nesbus& _mem)
{
	// Zero-page indirect addressing r_Y
	return (u8)(_mem[r_PC++] + r_Y);
}

void sfot::O_LDA(u16& _addr, nesbus& _mem)
{
	// Load accumulator
	r_A = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_LDX(u16& _addr, nesbus& _mem)
{
	// Load X register
	r_X = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_X);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_X >> (u8)r_SRSs::N);
}
void sfot::O_LDY(u16& _addr, nesbus& _mem)
{
	// Load Y register
	r_Y = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_Y);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_Y >> (u8)r_SRSs::N);
}
void sfot::O_STA(u16& _addr, nesbus& _mem)
{
	// Send accumulator contents to memory
	_mem[_addr] = (r_A);
}
void sfot::O_STX(u16& _addr, nesbus& _mem)
{
	// Send X register contents to memory
	_mem[_addr] = (r_X);
}
void sfot::O_STY(u16& _addr, nesbus& _mem)
{
	// Send Y register contents to memory
	_mem[_addr] = (r_Y);
}
void sfot::O_TAX(u16& _addr, nesbus& _mem)
{
	// Transfer accumulator to X register
	r_X = r_A;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_X);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_X >> (u8)r_SRSs::N);
}
void sfot::O_TAY(u16& _addr, nesbus& _mem)
{
	// Transfer accumulator to Y register
	r_Y = r_A;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_Y);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_Y >> (u8)r_SRSs::N);
}
void sfot::O_TXA(u16& _addr, nesbus& _mem)
{
	// Transfer X register to accumulator
	r_A = r_X;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_TYA(u16& _addr, nesbus& _mem)
{
	// Transfer Y register to accumulator
	r_A = r_Y;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_TSX(u16& _addr, nesbus& _mem)
{
	// Transfer stack pointer to X register
	r_X = r_S;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_X);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_X >> (u8)r_SRSs::N);
}
void sfot::O_TXS(u16& _addr, nesbus& _mem)
{
	// Transfer X register to stack pointer
	r_S = r_X;
}
void sfot::O_PHA(u16& _addr, nesbus& _mem)
{
	// Push accumulator to the stack
	u16 addr = 0x0100 | r_S; --r_S;
	_mem[addr] = (r_A);
}
void sfot::O_PHP(u16& _addr, nesbus& _mem)
{
	// Push status register to the stack
	u16 addr = 0x0100 | r_S; --r_S;
	_mem[addr] = (r_SR);
}
void sfot::O_PLA(u16& _addr, nesbus& _mem)
{
	// Pop top from stack to accumulator
	u16 addr = 0x0100 | ++r_S;
	r_A = _mem[addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_PLP(u16& _addr, nesbus& _mem)
{
	// Pop top from stack to status register
	u16 addr = 0x0100 | ++r_S;
	r_SR = _mem[addr] | (u8)r_SRS::S;
}
void sfot::O_AND(u16& _addr, nesbus& _mem)
{
	// Logical AND on accumulator and memory
	r_A &= _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_EOR(u16& _addr, nesbus& _mem)
{
	// Exclusive OR on accumulator and memory
	r_A ^= _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_ORA(u16& _addr, nesbus& _mem)
{
	// Inclusive OR on accumulator and memory
	r_A |= _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_BIT(u16& _addr, nesbus& _mem)
{
	// Bit test on accumulator and memory
	u8 test = r_A & _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !test);
	BIT_SET(r_SR, (u8)r_SRSs::O, !!(test & (u8)r_SRS::O));
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_ADC(u16& _addr, nesbus& _mem)
{
	// Add with carry (accumulator)
	u8 operand = _mem[_addr];
	u16 adc = r_A + operand + (r_SR & (u8)r_SRS::C);
	switch (r_SR & (u8)r_SRS::D)
	{
	case (u8)r_SRS::D: // Decimal mode
	{
		/* to-do: implement decimal mode */
	}
	default: // Binary mode
		// Set appropriate status flags
		BIT_SET(r_SR, (u8)r_SRSs::O, (~(r_A ^ operand) & (r_A ^ adc) & 0x80) >> 7);
		BIT_SET(r_SR, (u8)r_SRSs::C, adc > 0xFF);
		BIT_SET(r_SR, (u8)r_SRSs::N, adc >> (u8)r_SRSs::N);
		BIT_SET(r_SR, (u8)r_SRSs::Z, !(adc & 0xFF));
		// Set accumulator to sum
		r_A = adc & 0xFF;
		break;
	}
}
void sfot::O_SBC(u16& _addr, nesbus& _mem)
{
	// Subtract with carry (accumulator)
	// Invert the bits, little sneaky memory manip ;)
	_mem[_addr] = (~_mem[_addr]);
	sfot::O_ADC(_addr, _mem);
	_mem[_addr] = (~_mem[_addr]);
}
void sfot::O_CMP(u16& _addr, nesbus& _mem)
{
	// Compare accumulator with memory
	u8 operand = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::C, r_A >= operand);
	BIT_SET(r_SR, (u8)r_SRSs::Z, r_A == operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, !!((u8)(r_A - operand) & (u8)r_SRS::N));
}
void sfot::O_CPX(u16& _addr, nesbus& _mem)
{
	// Compare X register with memory
	u8 operand = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::C, r_X >= operand);
	BIT_SET(r_SR, (u8)r_SRSs::Z, r_X == operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, !!((u8)(r_X - operand) & (u8)r_SRS::N));
}
void sfot::O_CPY(u16& _addr, nesbus& _mem)
{
	// Compare Y register with memory
	u8 operand = _mem[_addr];
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::C, r_Y >= operand);
	BIT_SET(r_SR, (u8)r_SRSs::Z, r_Y == operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, !!((u8)(r_Y - operand) & (u8)r_SRS::N));
}
void sfot::O_INC(u16& _addr, nesbus& _mem)
{
	// Increment value at memory address
	u8 operand = _mem[_addr]; ++operand;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_INX(u16& _addr, nesbus& _mem)
{
	// Increment value in X register
	++r_X;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_X);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_X >> (u8)r_SRSs::N);
}
void sfot::O_INY(u16& _addr, nesbus& _mem)
{
	// Increment value in Y register
	++r_Y;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_Y);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_Y >> (u8)r_SRSs::N);
}
void sfot::O_DEC(u16& _addr, nesbus& _mem)
{
	// Decrement value at memory address
	u8 operand = _mem[_addr]; --operand;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_DEX(u16& _addr, nesbus& _mem)
{
	// Decrement value in X register
	--r_X;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_X);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_X >> (u8)r_SRSs::N);
}
void sfot::O_DEY(u16& _addr, nesbus& _mem)
{
	// Decrement value in Y register
	--r_Y;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_Y);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_Y >> (u8)r_SRSs::N);
}
void sfot::O_ASL(u16& _addr, nesbus& _mem)
{
	// Arithmetic shift left (memory)
	u8 operand = _mem[_addr];
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, (operand & 0x80) >> 7); operand <<= 1;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_ASL_A(u16& _addr, nesbus& _mem)
{
	// Arithmetic shift left (accumulator)
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, (r_A & 0x80) >> 7); r_A <<= 1;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_LSR(u16& _addr, nesbus& _mem)
{
	// Logical shift right (memory)
	u8 operand = _mem[_addr];
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, operand & 0x01); operand >>= 1;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_LSR_A(u16& _addr, nesbus& _mem)
{
	// Logical shift right (accumulator)
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, r_A & 0x01); r_A >>= 1;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_ROL(u16& _addr, nesbus& _mem)
{
	// Rotate left (memory)
	u8 operand = _mem[_addr];
	u8 oldCarry = r_SR & (u8)r_SRS::C;
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, (operand & 0x80) >> 7); operand <<= 1;
	operand |= oldCarry;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_ROL_A(u16& _addr, nesbus& _mem)
{
	// Rotate left (accumulator)
	u8 oldCarry = r_SR & (u8)r_SRS::C;
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, (r_A & 0x80) >> 7); r_A <<= 1;
	r_A |= oldCarry;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_ROR(u16& _addr, nesbus& _mem)
{
	// Rotate right (memory)
	u8 operand = _mem[_addr];
	u8 oldCarry = r_SR & (u8)r_SRS::C;
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, operand & 0x01); operand >>= 1;
	operand |= oldCarry;
	_mem[_addr] = (operand);
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !operand);
	BIT_SET(r_SR, (u8)r_SRSs::N, operand >> (u8)r_SRSs::N);
}
void sfot::O_ROR_A(u16& _addr, nesbus& _mem)
{
	// Rotate right (accumulator)
	u8 oldCarry = r_SR & (u8)r_SRS::C;
	// Set carry
	BIT_SET(r_SR, (u8)r_SRSs::C, r_A & 0x01); r_A >>= 1;
	r_A |= oldCarry;
	// Set appropriate status flags
	BIT_SET(r_SR, (u8)r_SRSs::Z, !r_A);
	BIT_SET(r_SR, (u8)r_SRSs::N, r_A >> (u8)r_SRSs::N);
}
void sfot::O_JMP(u16& _addr, nesbus& _mem)
{
	// Move program counter to specific address
	r_PC = _addr;
}
void sfot::O_JSR(u16& _addr, nesbus& _mem)
{
	// Jump to subroutine
	--r_PC;
	u16 addr = 0x0100 | r_S; --r_S;
	_mem[addr] = ((r_PC >> 8) & 0xFF);
	addr = 0x0100 | r_S; --r_S;
	_mem[addr] = (r_PC & 0xFF);
	r_PC = _addr;
}
void sfot::O_RTS(u16& _addr, nesbus& _mem)
{
	// Return from subroutine, get from stack
	u16 addr = 0x0100 | ++r_S;
	r_PC = _mem[addr]; 
	addr = 0x0100 | ++r_S;
	r_PC |= (_mem[addr]) << 8; ++r_PC;
}
void sfot::O_BCC(u16& _addr, nesbus& _mem)
{
	// Branch if carry is clear
	r_PC = (!(r_SR & (u8)r_SRS::C) ? _addr : r_PC);
}
void sfot::O_BCS(u16& _addr, nesbus& _mem)
{
	// Branch if carry is set
	r_PC = (!(r_SR & (u8)r_SRS::C) ? r_PC : _addr);
}
void sfot::O_BEQ(u16& _addr, nesbus& _mem)
{
	// Branch if equal (zero flag set)
	r_PC = (!(r_SR & (u8)r_SRS::Z) ? r_PC : _addr);
}
void sfot::O_BMI(u16& _addr, nesbus& _mem)
{
	// Branch if minus (negative flag set)
	r_PC = (!(r_SR & (u8)r_SRS::N) ? r_PC : _addr);
}
void sfot::O_BNE(u16& _addr, nesbus& _mem)
{
	// Branch if not equal (zero flag clear)
	r_PC = (!(r_SR & (u8)r_SRS::Z) ? _addr : r_PC);
}
void sfot::O_BPL(u16& _addr, nesbus& _mem)
{
	// Branch if positive (negative flag clear)
	r_PC = (!(r_SR & (u8)r_SRS::N) ? _addr : r_PC);
}
void sfot::O_BVC(u16& _addr, nesbus& _mem)
{
	// Branch if overflow clear
	r_PC = (!(r_SR & (u8)r_SRS::O) ? _addr : r_PC);
}
void sfot::O_BVS(u16& _addr, nesbus& _mem)
{
	// Branch if overflow set
	r_PC = (!(r_SR & (u8)r_SRS::O) ? r_PC : _addr);
}
void sfot::O_CLC(u16& _addr, nesbus& _mem)
{
	// Clear carry
	r_SR &= ~(u8)r_SRS::C;
}
void sfot::O_CLD(u16& _addr, nesbus& _mem)
{
	// Clear decimal mode
	r_SR &= ~(u8)r_SRS::D;
}
void sfot::O_CLI(u16& _addr, nesbus& _mem)
{
	// Clear interrupt disable
	r_SR &= ~(u8)r_SRS::I;
}
void sfot::O_CLV(u16& _addr, nesbus& _mem)
{
	// Clear overflow
	r_SR &= ~(u8)r_SRS::O;
}
void sfot::O_SEC(u16& _addr, nesbus& _mem)
{
	// Set carry
	r_SR |= (u8)r_SRS::C;
}
void sfot::O_SED(u16& _addr, nesbus& _mem)
{
	// Set decimal mode
	r_SR |= (u8)r_SRS::D;
}
void sfot::O_SEI(u16& _addr, nesbus& _mem)
{
	// Set interrupt disable
	r_SR |= (u8)r_SRS::I;
}
void sfot::O_BRK(u16& _addr, nesbus& _mem)
{
	// Break (force an interrupt)
	// Save PC and SR to stack
	u16 addr = 0x0100 | r_S; --r_S;
	_mem[addr] = ((r_PC >> 8) & 0xFF);
	addr = 0x0100 | r_S; --r_S;
	_mem[addr] = (r_PC & 0xFF);
	addr = 0x0100 | r_S; --r_S;
	_mem[addr] = (r_SR);
	// Go to location
	r_PC = _mem[e_BRK_L] + (_mem[e_BRK_H] << 8);
	// Set appropriate status flags
	r_SR |= (u8)r_SRS::B;
}
void sfot::O_RTI(u16& _addr, nesbus& _mem)
{
	// Return from interrupt
	// Get SR from stack
	u16 addr = 0x0100 | ++r_S;
	r_SR = _mem[addr];
	// Get PC from stack
	addr = 0x0100 | ++r_S;
	r_PC = _mem[addr];
	addr = 0x0100 | ++r_S;
	r_PC |= (_mem[addr]) << 8; 
}

u64 sfot::EmulateCycles(nesbus& _memory, u64& _cycleAmount)
{
	while (e_Cycles < _cycleAmount)
	{
		EmulateStep(_memory);
	}
	return (e_Cycles - _cycleAmount);
}

void sfot::EmulateStep(nesbus& _memory)
{
	// Get opcode from memory block
	u8 opcode = _memory[r_PC]; ++r_PC;

	u16 addr = 0;
	switch (e_OCAM[opcode])
	{
	case (u8)r_AM::IMM: { addr = r_PC; ++r_PC; break; } // Immediate addressing
	case (u8)r_AM::NINST: { return;  break; } // No instruction to execute (NOP & out of mem)
	case (u8)r_AM::NOADDR: { break; } // No address needed 
	default:
		// Get the address from the opcodes addressing mode
		addr = (this->*e_AMJT[e_OCAM[opcode]])(_memory);
	}

	// Execute the instruction
	(this->*e_OCJT[opcode])(addr, _memory);
}

sfot::sfot()
{
	// Set OCAM & OCJT to NoInsts
	for (u32 i = 0; i < 256; ++i) { e_OCAM[i] = (u8)r_AM::NINST; e_OCJT[i] = 0; }

	// Set up adressing jump-table
	e_AMJT[0] = &sfot::AM_Abs;
	e_AMJT[1] = &sfot::AM_AbsX;
	e_AMJT[2] = &sfot::AM_AbsY;
	e_AMJT[3] = &sfot::AM_Ind;
	e_AMJT[4] = &sfot::AM_XInd;
	e_AMJT[5] = &sfot::AM_IndY;
	e_AMJT[6] = &sfot::AM_Rel;
	e_AMJT[7] = &sfot::AM_Zpg;
	e_AMJT[8] = &sfot::AM_ZpgX;
	e_AMJT[9] = &sfot::AM_ZpgY;

	// Set up instruction jump-table & opcode addressing
		// Load/store operations
	// Load accumulator
	e_OCJT[(u8)sfotops::LDA_IMM] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_IMM] = (u8)r_AM::IMM;

	e_OCJT[(u8)sfotops::LDA_ZPG] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_ZPG] = (u8)r_AM::ZPG;

	e_OCJT[(u8)sfotops::LDA_ZPGX] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_ZPGX] = (u8)r_AM::ZPGX;

	e_OCJT[(u8)sfotops::LDA_ABS] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_ABS] = (u8)r_AM::ABS;

	e_OCJT[(u8)sfotops::LDA_ABSX] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_ABSX] = (u8)r_AM::ABSX;

	e_OCJT[(u8)sfotops::LDA_ABSY] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_ABSY] = (u8)r_AM::ABSY;

	e_OCJT[(u8)sfotops::LDA_XIND] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_XIND] = (u8)r_AM::XIND;

	e_OCJT[(u8)sfotops::LDA_INDY] = &sfot::O_LDA;
	e_OCAM[(u8)sfotops::LDA_INDY] = (u8)r_AM::INDY;

	// Load X register
	e_OCJT[(u8)sfotops::LDX_IMM] = &sfot::O_LDX;
	e_OCAM[(u8)sfotops::LDX_IMM] = (u8)r_AM::IMM;
						  
	e_OCJT[(u8)sfotops::LDX_ZPG] = &sfot::O_LDX;
	e_OCAM[(u8)sfotops::LDX_ZPG] = (u8)r_AM::ZPG;
						  
	e_OCJT[(u8)sfotops::LDX_ZPGY] = &sfot::O_LDX;
	e_OCAM[(u8)sfotops::LDX_ZPGY] = (u8)r_AM::ZPGY;
						  
	e_OCJT[(u8)sfotops::LDX_ABS] = &sfot::O_LDX;
	e_OCAM[(u8)sfotops::LDX_ABS] = (u8)r_AM::ABS;
						  
	e_OCJT[(u8)sfotops::LDX_ABSY] = &sfot::O_LDX;
	e_OCAM[(u8)sfotops::LDX_ABSY] = (u8)r_AM::ABSY;

	// Load Y register
	e_OCJT[(u8)sfotops::LDY_IMM] = &sfot::O_LDY;
	e_OCAM[(u8)sfotops::LDY_IMM] = (u8)r_AM::IMM;
						  
	e_OCJT[(u8)sfotops::LDY_ZPG] = &sfot::O_LDY;
	e_OCAM[(u8)sfotops::LDY_ZPG] = (u8)r_AM::ZPG;
						  
	e_OCJT[(u8)sfotops::LDY_ZPGX] = &sfot::O_LDY;
	e_OCAM[(u8)sfotops::LDY_ZPGX] = (u8)r_AM::ZPGX;
						  
	e_OCJT[(u8)sfotops::LDY_ABS] = &sfot::O_LDY;
	e_OCAM[(u8)sfotops::LDY_ABS] = (u8)r_AM::ABS;
						  
	e_OCJT[(u8)sfotops::LDY_ABSX] = &sfot::O_LDY;
	e_OCAM[(u8)sfotops::LDY_ABSX] = (u8)r_AM::ABSX;

	// Store accumulator to memory
	e_OCJT[(u8)sfotops::STA_ZPG] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::STA_ZPGX] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::STA_ABS] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::STA_ABSX] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::STA_ABSY] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::STA_XIND] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::STA_INDY] = &sfot::O_STA;
	e_OCAM[(u8)sfotops::STA_INDY] = (u8)r_AM::INDY;

	// Store X register to memory
	e_OCJT[(u8)sfotops::STX_ZPG] = &sfot::O_STX;
	e_OCAM[(u8)sfotops::STX_ZPG] = (u8)r_AM::ZPG;
						  
	e_OCJT[(u8)sfotops::STX_ZPGY] = &sfot::O_STX;
	e_OCAM[(u8)sfotops::STX_ZPGY] = (u8)r_AM::ZPGY;
						  
	e_OCJT[(u8)sfotops::STX_ABS] = &sfot::O_STX;
	e_OCAM[(u8)sfotops::STX_ABS] = (u8)r_AM::ABS;

	// Store Y register to memory
	e_OCJT[(u8)sfotops::STY_ZPG] = &sfot::O_STY;
	e_OCAM[(u8)sfotops::STY_ZPG] = (u8)r_AM::ZPG;
						  
	e_OCJT[(u8)sfotops::STY_ZPGX] = &sfot::O_STY;
	e_OCAM[(u8)sfotops::STY_ZPGX] = (u8)r_AM::ZPGX;
						  
	e_OCJT[(u8)sfotops::STY_ABS] = &sfot::O_STY;
	e_OCAM[(u8)sfotops::STY_ABS] = (u8)r_AM::ABS;

		// Register transfers
	// Copy accumulator to X register
	e_OCJT[(u8)sfotops::TAX] = &sfot::O_TAX;
	e_OCAM[(u8)sfotops::TAX] = (u8)r_AM::NOADDR;

	// Copy accumulator to Y register
	e_OCJT[(u8)sfotops::TAY] = &sfot::O_TAY;
	e_OCAM[(u8)sfotops::TAY] = (u8)r_AM::NOADDR;

	// Copy X register to accumulator
	e_OCJT[(u8)sfotops::TXA] = &sfot::O_TXA;
	e_OCAM[(u8)sfotops::TXA] = (u8)r_AM::NOADDR;

	// Copy Y register to accumulator
	e_OCJT[(u8)sfotops::TYA] = &sfot::O_TYA;
	e_OCAM[(u8)sfotops::TYA] = (u8)r_AM::NOADDR;

		// Stack operations
	// Copy stack pointer to X register
	e_OCJT[(u8)sfotops::TSX] = &sfot::O_TSX;
	e_OCAM[(u8)sfotops::TSX] = (u8)r_AM::NOADDR;

	// Copy X register to stack pointer
	e_OCJT[(u8)sfotops::TXS] = &sfot::O_TXS;
	e_OCAM[(u8)sfotops::TXS] = (u8)r_AM::NOADDR;

	// Push accumulator to stack
	e_OCJT[(u8)sfotops::PHA] = &sfot::O_PHA;
	e_OCAM[(u8)sfotops::PHA] = (u8)r_AM::NOADDR;

	// Push status register to stack
	e_OCJT[(u8)sfotops::PHP] = &sfot::O_PHP;
	e_OCAM[(u8)sfotops::PHP] = (u8)r_AM::NOADDR;

	// Pop to accumulator
	e_OCJT[(u8)sfotops::PLA] = &sfot::O_PLA;
	e_OCAM[(u8)sfotops::PLA] = (u8)r_AM::NOADDR;

	// Pop to status register
	e_OCJT[(u8)sfotops::PLP] = &sfot::O_PLP;
	e_OCAM[(u8)sfotops::PLP] = (u8)r_AM::NOADDR;

		// Logical
	// Logical AND with accumulator and memory
	e_OCJT[(u8)sfotops::AND_IMM] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::AND_ZPG] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::AND_ZPGX] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::AND_ABS] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::AND_ABSX] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::AND_ABSY] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::AND_XIND] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::AND_INDY] = &sfot::O_AND;
	e_OCAM[(u8)sfotops::AND_INDY] = (u8)r_AM::INDY;

	// Exclusive OR with accumulator and memory
	e_OCJT[(u8)sfotops::EOR_IMM] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::EOR_ZPG] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::EOR_ZPGX] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::EOR_ABS] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::EOR_ABSX] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::EOR_ABSY] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::EOR_XIND] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::EOR_INDY] = &sfot::O_EOR;
	e_OCAM[(u8)sfotops::EOR_INDY] = (u8)r_AM::INDY;

	// Inclusive OR with accumulator and memory
	e_OCJT[(u8)sfotops::ORA_IMM] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::ORA_ZPG] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::ORA_ZPGX] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::ORA_ABS] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::ORA_ABSX] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::ORA_ABSY] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::ORA_XIND] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::ORA_INDY] = &sfot::O_ORA;
	e_OCAM[(u8)sfotops::ORA_INDY] = (u8)r_AM::INDY;

	// Bit test with accumulator and memory
	e_OCJT[(u8)sfotops::BIT_ZPG] = &sfot::O_BIT;
	e_OCAM[(u8)sfotops::BIT_ZPG] = (u8)r_AM::ZPG;

	e_OCJT[(u8)sfotops::BIT_ABS] = &sfot::O_BIT;
	e_OCAM[(u8)sfotops::BIT_ABS] = (u8)r_AM::ABS;

		// Arithmetic
	// Add with carry to accumulator
	e_OCJT[(u8)sfotops::ADC_IMM] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::ADC_ZPG] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::ADC_ZPGX] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::ADC_ABS] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::ADC_ABSX] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::ADC_ABSY] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::ADC_XIND] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::ADC_INDY] = &sfot::O_ADC;
	e_OCAM[(u8)sfotops::ADC_INDY] = (u8)r_AM::INDY;

	// Subtract with carry to accumulator
	e_OCJT[(u8)sfotops::SBC_IMM] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::SBC_ZPG] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::SBC_ZPGX] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::SBC_ABS] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::SBC_ABSX] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::SBC_ABSY] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::SBC_XIND] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::SBC_INDY] = &sfot::O_SBC;
	e_OCAM[(u8)sfotops::SBC_INDY] = (u8)r_AM::INDY;

	// Compare accumulator to memory
	e_OCJT[(u8)sfotops::CMP_IMM] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_IMM] = (u8)r_AM::IMM;
						
	e_OCJT[(u8)sfotops::CMP_ZPG] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::CMP_ZPGX] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::CMP_ABS] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::CMP_ABSX] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_ABSX] = (u8)r_AM::ABSX;
						
	e_OCJT[(u8)sfotops::CMP_ABSY] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_ABSY] = (u8)r_AM::ABSY;
						
	e_OCJT[(u8)sfotops::CMP_XIND] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_XIND] = (u8)r_AM::XIND;
						
	e_OCJT[(u8)sfotops::CMP_INDY] = &sfot::O_CMP;
	e_OCAM[(u8)sfotops::CMP_INDY] = (u8)r_AM::INDY;

	// Compare X register to memory
	e_OCJT[(u8)sfotops::CPX_IMM] = &sfot::O_CPX;
	e_OCAM[(u8)sfotops::CPX_IMM] = (u8)r_AM::IMM;
						 
	e_OCJT[(u8)sfotops::CPX_ZPG] = &sfot::O_CPX;
	e_OCAM[(u8)sfotops::CPX_ZPG] = (u8)r_AM::ZPG;
						 
	e_OCJT[(u8)sfotops::CPX_ABS] = &sfot::O_CPX;
	e_OCAM[(u8)sfotops::CPX_ABS] = (u8)r_AM::ABS;

	// Compare Y register to memory
	e_OCJT[(u8)sfotops::CPY_IMM] = &sfot::O_CPY;
	e_OCAM[(u8)sfotops::CPY_IMM] = (u8)r_AM::IMM;
						  
	e_OCJT[(u8)sfotops::CPY_ZPG] = &sfot::O_CPY;
	e_OCAM[(u8)sfotops::CPY_ZPG] = (u8)r_AM::ZPG;
						  
	e_OCJT[(u8)sfotops::CPY_ABS] = &sfot::O_CPY;
	e_OCAM[(u8)sfotops::CPY_ABS] = (u8)r_AM::ABS;
	
		// Increments & decrements
	// Increment memory
	e_OCJT[(u8)sfotops::INC_ZPG] = &sfot::O_INC;
	e_OCAM[(u8)sfotops::INC_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::INC_ZPGX] = &sfot::O_INC;
	e_OCAM[(u8)sfotops::INC_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::INC_ABS] = &sfot::O_INC;
	e_OCAM[(u8)sfotops::INC_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::INC_ABSX] = &sfot::O_INC;
	e_OCAM[(u8)sfotops::INC_ABSX] = (u8)r_AM::ABSX;

	// Increment X register
	e_OCJT[(u8)sfotops::INX] = &sfot::O_INX;
	e_OCAM[(u8)sfotops::INX] = (u8)r_AM::NOADDR;

	// Increment Y register
	e_OCJT[(u8)sfotops::INY] = &sfot::O_INY;
	e_OCAM[(u8)sfotops::INY] = (u8)r_AM::NOADDR;

	// Decrement memory
	e_OCJT[(u8)sfotops::DEC_ZPG] = &sfot::O_DEC;
	e_OCAM[(u8)sfotops::DEC_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::DEC_ZPGX] = &sfot::O_DEC;
	e_OCAM[(u8)sfotops::DEC_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::DEC_ABS] = &sfot::O_DEC;
	e_OCAM[(u8)sfotops::DEC_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::DEC_ABSX] = &sfot::O_DEC;
	e_OCAM[(u8)sfotops::DEC_ABSX] = (u8)r_AM::ABSX;

	// Decrement X register
	e_OCJT[(u8)sfotops::DEX] = &sfot::O_DEX;
	e_OCAM[(u8)sfotops::DEX] = (u8)r_AM::NOADDR;

	// Decrement Y register
	e_OCJT[(u8)sfotops::DEY] = &sfot::O_DEY;
	e_OCAM[(u8)sfotops::DEY] = (u8)r_AM::NOADDR;

		// Shifts
	// Arithmetic shift left
	e_OCJT[(u8)sfotops::ASL_A] = &sfot::O_ASL_A;
	e_OCAM[(u8)sfotops::ASL_A] = (u8)r_AM::NOADDR;

	e_OCJT[(u8)sfotops::ASL_ZPG] = &sfot::O_ASL;
	e_OCAM[(u8)sfotops::ASL_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::ASL_ZPGX] = &sfot::O_ASL;
	e_OCAM[(u8)sfotops::ASL_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::ASL_ABS] = &sfot::O_ASL;
	e_OCAM[(u8)sfotops::ASL_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::ASL_ABSX] = &sfot::O_ASL;
	e_OCAM[(u8)sfotops::ASL_ABSX] = (u8)r_AM::ABSX;

	// Logical shift right
	e_OCJT[(u8)sfotops::LSR_A] = &sfot::O_LSR_A;
	e_OCAM[(u8)sfotops::LSR_A] = (u8)r_AM::NOADDR;
						
	e_OCJT[(u8)sfotops::LSR_ZPG] = &sfot::O_LSR;
	e_OCAM[(u8)sfotops::LSR_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::LSR_ZPGX] = &sfot::O_LSR;
	e_OCAM[(u8)sfotops::LSR_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::LSR_ABS] = &sfot::O_LSR;
	e_OCAM[(u8)sfotops::LSR_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::LSR_ABSX] = &sfot::O_LSR;
	e_OCAM[(u8)sfotops::LSR_ABSX] = (u8)r_AM::ABSX;

	// Rotate left
	e_OCJT[(u8)sfotops::ROL_A] = &sfot::O_ROL_A;
	e_OCAM[(u8)sfotops::ROL_A] = (u8)r_AM::NOADDR;
						
	e_OCJT[(u8)sfotops::ROL_ZPG] = &sfot::O_ROL;
	e_OCAM[(u8)sfotops::ROL_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::ROL_ZPGX] = &sfot::O_ROL;
	e_OCAM[(u8)sfotops::ROL_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::ROL_ABS] = &sfot::O_ROL;
	e_OCAM[(u8)sfotops::ROL_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::ROL_ABSX] = &sfot::O_ROL;
	e_OCAM[(u8)sfotops::ROL_ABSX] = (u8)r_AM::ABSX;

	// Rotate right
	e_OCJT[(u8)sfotops::ROR_A] = &sfot::O_ROR_A;
	e_OCAM[(u8)sfotops::ROR_A] = (u8)r_AM::NOADDR;
						
	e_OCJT[(u8)sfotops::ROR_ZPG] = &sfot::O_ROR;
	e_OCAM[(u8)sfotops::ROR_ZPG] = (u8)r_AM::ZPG;
						
	e_OCJT[(u8)sfotops::ROR_ZPGX] = &sfot::O_ROR;
	e_OCAM[(u8)sfotops::ROR_ZPGX] = (u8)r_AM::ZPGX;
						
	e_OCJT[(u8)sfotops::ROR_ABS] = &sfot::O_ROR;
	e_OCAM[(u8)sfotops::ROR_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::ROR_ABSX] = &sfot::O_ROR;
	e_OCAM[(u8)sfotops::ROR_ABSX] = (u8)r_AM::ABSX;

		// Jumps & calls
	// Move program counter to specific address
	e_OCJT[(u8)sfotops::JMP_ABS] = &sfot::O_JMP;
	e_OCAM[(u8)sfotops::JMP_ABS] = (u8)r_AM::ABS;
						
	e_OCJT[(u8)sfotops::JMP_IND] = &sfot::O_JMP;
	e_OCAM[(u8)sfotops::JMP_IND] = (u8)r_AM::IND;

	// Jump to subroutine
	e_OCJT[(u8)sfotops::JSR_ABS] = &sfot::O_JSR;
	e_OCAM[(u8)sfotops::JSR_ABS] = (u8)r_AM::ABS;

	// Return from subroutine
	e_OCJT[(u8)sfotops::RTS] = &sfot::O_RTS;
	e_OCAM[(u8)sfotops::RTS] = (u8)r_AM::NOADDR;

		// Branches
	// Branch if carry clear
	e_OCJT[(u8)sfotops::BCC] = &sfot::O_BCC;
	e_OCAM[(u8)sfotops::BCC] = (u8)r_AM::REL;

	// Branch if carry set
	e_OCJT[(u8)sfotops::BCS] = &sfot::O_BCS;
	e_OCAM[(u8)sfotops::BCS] = (u8)r_AM::REL;

	// Branch if equal
	e_OCJT[(u8)sfotops::BEQ] = &sfot::O_BEQ;
	e_OCAM[(u8)sfotops::BEQ] = (u8)r_AM::REL;

	// Branch if minus
	e_OCJT[(u8)sfotops::BMI] = &sfot::O_BMI;
	e_OCAM[(u8)sfotops::BMI] = (u8)r_AM::REL;

	// Branch if not equal
	e_OCJT[(u8)sfotops::BNE] = &sfot::O_BNE;
	e_OCAM[(u8)sfotops::BNE] = (u8)r_AM::REL;

	// Branch if positive
	e_OCJT[(u8)sfotops::BPL] = &sfot::O_BPL;
	e_OCAM[(u8)sfotops::BPL] = (u8)r_AM::REL;

	// Branch if overflow clear
	e_OCJT[(u8)sfotops::BVC] = &sfot::O_BVC;
	e_OCAM[(u8)sfotops::BVC] = (u8)r_AM::REL;

	// Branch if overflow set
	e_OCJT[(u8)sfotops::BVS] = &sfot::O_BVS;
	e_OCAM[(u8)sfotops::BVS] = (u8)r_AM::REL;

		// Status flag changes
	// Clear carry 
	e_OCJT[(u8)sfotops::CLC] = &sfot::O_CLC;
	e_OCAM[(u8)sfotops::CLC] = (u8)r_AM::NOADDR;

	// Clear decimal mode
	e_OCJT[(u8)sfotops::CLD] = &sfot::O_CLD;
	e_OCAM[(u8)sfotops::CLD] = (u8)r_AM::NOADDR;

	// Clear interrupt disable
	e_OCJT[(u8)sfotops::CLI] = &sfot::O_CLI;
	e_OCAM[(u8)sfotops::CLI] = (u8)r_AM::NOADDR;

	// Clear overflow
	e_OCJT[(u8)sfotops::CLV] = &sfot::O_CLV;
	e_OCAM[(u8)sfotops::CLV] = (u8)r_AM::NOADDR;

	// Set carry
	e_OCJT[(u8)sfotops::SEC] = &sfot::O_SEC;
	e_OCAM[(u8)sfotops::SEC] = (u8)r_AM::NOADDR;

	// Set decimal mode
	e_OCJT[(u8)sfotops::SED] = &sfot::O_SED;
	e_OCAM[(u8)sfotops::SED] = (u8)r_AM::NOADDR;

	// Set interrupt disable
	e_OCJT[(u8)sfotops::SEI] = &sfot::O_SEI;
	e_OCAM[(u8)sfotops::SEI] = (u8)r_AM::NOADDR;


		// System functions
	// Break (software interrupt)
	e_OCJT[(u8)sfotops::BRK] = &sfot::O_BRK;
	e_OCAM[(u8)sfotops::BRK] = (u8)r_AM::NOADDR;

	// No Operation
	e_OCJT[(u8)sfotops::NOP] = 0;
	e_OCAM[(u8)sfotops::NOP] = (u8)r_AM::NINST;

	// Return from interrupt
	e_OCJT[(u8)sfotops::RTI] = &sfot::O_RTI;
	e_OCAM[(u8)sfotops::RTI] = (u8)r_AM::NOADDR;

}