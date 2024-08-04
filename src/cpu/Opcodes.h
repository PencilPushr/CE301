#ifndef OPCODE_H
#define OPCODE_H

#include <string>
#include "Error.h"
#include "Registers.h"

class Memory;
class CPU;

/*
static void setFlags(CPU* cpu, uint16_t reg, uint16_t reg2 = 0)
{
    cpu->flags.Z = (reg == 0) & 1;  // Set Z flag to 1 if A is zero, else 0
    cpu->flags.S = (reg > 0) & 1;   // Set S flag to 1 if A is positive, else 0
    cpu->flags.C = (((uint16_t)reg + reg2) > 0xff) & 1; //set C flag to 1 if the result of the operation is going to be too big to fit in the register
    cpu->flags.P = ((std::popcount(reg) % 2) == 0) & 1; //set the Parity flag to 1 if number of bits is even, 0 if odd
    //cpu->flags.AC = ac_flagADD(cpu->A, cpu->B);
}
*/
/*

Add a function called dissasemble_opcode_data(), it will make your life easier, as we just need to print that out next.


*/


static void setZF(CPU* cpu, uint32_t a)
{
    cpu->eflags.ZF = (a == 0) & 1;  // Set Z flag to 1 if reg is zero, else 0
}
static void setS(CPU* cpu, uint32_t a)
{   //0x80 is 128 which is the halfway point for a 8bit number, which means it acts as 0. This was because ((0xFF / 2) + 1) = 0x80
    //
    //uint16MAX is equivelant to 0x0000'FFFF
    cpu->eflags.SF = (a > UINT16_MAX) & 1;   // Set S flag to 1 if A is positive, else 0
}
static void setCF(CPU* cpu, uint32_t reg, uint32_t reg2 = 0)
{
    cpu->eflags.CF = (((uint32_t)reg + reg2) > 0xffff) & 1; //set C flag to 1 if the result of the operation is going to be too big to fit in the register
}
static void setP(CPU* cpu, uint32_t reg)
{
    cpu->eflags.PF = ((__popcnt(reg) % 2) == 0) & 1; //set the Parity flag to 1 if number of bits is even, 0 if odd
}

static uint8_t ac_flagADD(uint8_t a, uint8_t b)
{
    uint8_t lowNibbleA = a & 0x0F;
    uint8_t lowNibbleB = b & 0x0F;
    uint8_t carry = (lowNibbleA + lowNibbleB) > 0x0F;
    return carry;
}

#endif // OPCODE_H