#ifndef CPU_H
#define CPU_H

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include "Timer.h"
#include "Registers.h"
#include "Error.h"
#include <Zydis/Zydis.h>

class Memory;

class CPU
{
public:

	// Main Registers
	//Registers AH, AL are 8 bits, but can be combined into 16 bit registers

	//GENERAL PURPOSE REGISTERS

	//remember to add functions that only allow you to access the lower order registers
	union
	{
		//maybe do struct private, and expose them via functions
		struct
		{
			union {

				struct {
					//1111 0000
					uint8_t AH : 8;	//1111
					uint8_t AL : 8; //0000
				};
				uint16_t AX : 16;
			};
		};
		uint32_t EAX = 0;
	};

	union
	{
		struct
		{
			union
			{
				struct
				{
					//1111 0000
					uint8_t BH : 8;	//1111
					uint8_t BL : 8; //0000
				};
				uint16_t BX : 16;
			};
		};
		uint32_t EBX = 0;
	};

	union
	{
		struct
		{
			union
			{
				struct
				{
					//1111 0000
					uint8_t CH : 8;	//1111
					uint8_t CL : 8; //0000
				};
				uint16_t CX : 16;
			};
		};
		uint32_t ECX = 0;
	};

	union
	{
		struct
		{
			union
			{
				//1111 0000
				struct
				{
					uint8_t DH : 8;	//1111
					uint8_t DL : 8; //0000
				};
				uint16_t DX : 16;
			};
		};
		uint32_t EDX = 0;
	};

	//INDEX REGISTERS

	union
	{
		uint16_t SI : 16;
		uint32_t ESI = 0;
	};

	union
	{
		uint16_t DI : 16;
		uint32_t EDI = 0;
	};

	union
	{
		uint16_t BP : 16;
		uint32_t EBP = 0;
	};

	union
	{
		uint16_t SP : 16;
		uint32_t ESP = 0;
	};

	union
	{
		// Program Counter
		//uint32_t PC = 0;
		uint16_t IP : 16;
		uint32_t EIP = 0;
	};

	void* StackAllocation;

	union eFLAGS 
	{
		struct 
		{
			uint32_t CF : 1; // Carry Flag set if the last addition operation resulted in a carry or if the last subtraction operation required a borrow
			uint32_t RESERVED1 : 1; // Unused

			uint32_t PF : 1; // Parity Flag set if the number of 1 bits in the result is even
			uint32_t RESERVED2 : 1; // Unused

			uint32_t AF : 1; // Auxiliary Carry Flag used for binary-coded decimal arithmetic (not used in x86)
			uint32_t RESERVED3 : 1; // Unused

			uint32_t ZF : 1; // Zero Flag set if the result is zero
			uint32_t SF : 1; // Sign Flag set if the result is negative
			uint32_t TF : 1; // Trap Flag set to enable single-step mode for debugging
			uint32_t IF : 1; // Interrupt Enable Flag set to enable interrupts
			uint32_t DF : 1; // Direction Flag used by some string instructions
			uint32_t OF : 1; // Overflow Flag set if the signed result is too large to fit in the destination operand
			uint32_t IOPL : 2; // I/O Privilege Level (used by protected mode)
			uint32_t NT : 1; // Nested Task Flag (used by protected mode)
			uint32_t RF : 1; // Resume Flag (used by protected mode)
			uint32_t VM : 1; // Virtual 8086 Mode flag (used by protected mode)

			uint32_t RESERVED4 : 12; // Unused
		};

		uint32_t value = 0;
	};


	std::array<uint8_t, 0xFFFF>* m_memory;

	eFLAGS eflags;

	//i386 has a frequency range of approx 20-33 Mhz
	int m_processer_frequency = 20'000'000;
	//int m_processer_frequency = 2000000; // 2_000_000 or 2 Mhz

public:
	CPU();
	void load_register_value(registers& reg, uint32_t* value);
	void execute_opcode(ZydisDecodedInstruction* cur_op, Memory* mem);
	void setFlags(const ZydisDecodedInstruction& instruction);
	void startExecAt(Memory* mem);
	void toggleStart();
	void reset();

public:
	static std::string RegToStr(int r);

public:
	//turn on if game/instructions needs to be frame time limited.
	static constexpr bool LimitCycles = true;

private:
	ErrorCode cpu_LastError;
	Timer timer;
	static bool start;
	//this is done to avoid using a constructor/destructor, and thus we just call the functions themselves. 
	//if opcode* is used, we would need to use a constructor and destructor to avoid memory leaks
	//std::shared_ptr<opcode> opcodeLookup[256]; //this equiveleant to opcode* opcodeLookup[255]
};
#endif

/*
union eFLAGS
	{
		struct
		{
			uint8_t AF : 1; // Auxiliary Flag set when there is a carry or borrow after a nibble addition or subtraction
			uint8_t SF : 1; // Sign Flag set if the result is negative
			uint8_t ZF : 1; // Zero Flag set if the result is zero.
			uint8_t AC : 1; // Half-Carry Flag used for binary-coded decimal arithmetic
			uint8_t PF : 1; // Parity Flag set if the number of 1 bits in the result is even
			uint8_t CF : 1; // Carry Flag set if the last addition operation resulted in a carry or if the last subtraction operation required a borrow
			uint8_t OF : 1; // Overflow Flag set if result of SIGNED operation is too large to fit in number of bits, or if value "spilt" over/under.
			uint8_t RESERVED : 3; // Unused
		};

		uint8_t value = 0;
	};
	*/
