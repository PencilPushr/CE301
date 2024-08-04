#include "CPU.h"

#include <chrono>
#include <thread>


CPU::CPU()
{
	start = false;

	this->StackAllocation = malloc(0x1000);
	this->ESP = 0x1000;

	memset(this->StackAllocation, 0, 0x1000);

	printf("stack initialised at %p with mem %p\n\n", this->ESP, this->StackAllocation);
}

//assumes memory has been loaded into ram (think of disk being loaded into memory)
void CPU::startExecAt(Memory* mem)
{

}

void CPU::execute_opcode(ZydisDecodedInstruction* cur_op, Memory* mem)
{
	/*
	// To use the instruction information (the actual machine language) use:
	// instruction.info

	while (!start) {}

	//opcode* cur_op = opcodeLookup[index].get(); 
	// DO NOT USE .GET() IT USES A TEMPORARY AS A RETURN VALUE
	// THAT MEANS YOU THEN TRY TO ASSIGN A DESTROYED TEMPORARY WHEN YOU EXIT THE SCOPE OF .GET()
	//std::shared_ptr<opcode> cur_op = opcodeLookup[index];
	if constexpr (LimitCycles)
	{
		timer.Reset();
		cur_op->execute(this, mem);
		int cyclefreq = (1 / this->m_processer_frequency) * 1000000000; //this should be done at compile time FIX IT!
		this->EIP += cur_op->m_size;
		timer.Sleep((cyclefreq * cur_op->m_cycle_duration) - timer.Time()); //sleep for the correct amount of "fake" cycles
		//add timer.accumulate, so we can see how long it takes to execute every instruction
	}
	else
	{
		cur_op->execute(this, mem);
		this->EIP += cur_op->m_size;
	}
	//return cur_op;
	*/
}

void CPU::setFlags(const ZydisDecodedInstruction& instruction)
{
	// Get the instruction's meta information
	CPU::eflags.value = instruction.cpu_flags_written;
	//const eFLAGS& info = ZydisGetAccessedFlagsWritten(&instruction);

	// Set the individual flags based on the affected flags
	//eflags.CF = instruction.has.hasCfWrite;
	//eflags.PF = info.hasPfWrite;
	//eflags.AF = info.hasAfWrite;
	//eflags.ZF = info.hasZfWrite;
	//eflags.SF = info.hasSfWrite;
	//eflags.TF = info.hasTfWrite;
	//eflags.IF = info.hasIfWrite;
	//eflags.DF = info.hasDfWrite;
	//eflags.OF = info.hasOfWrite;
	//
	//// Extract other flags as necessary
	//eflags.IOPL = (instruction.attributes & ZYDIS_ATTRIB_IOPL_MASK) >> ZYDIS_ATTRIB_IOPL_SHIFT;
	//eflags.NT = (instruction.attributes & ZYDIS_ATTRIB_NT) != 0;
	//eflags.RF = (instruction.attributes & ZYDIS_ATTRIB_RF) != 0;
	//eflags.VM = (instruction.attributes & ZYDIS_ATTRIB_VM) != 0;
}

std::string CPU::RegToStr(int r)
{
	static const char* reg[] = {
		"EAX", "AX", "AH", "AL",
		"EBX", "BX", "BH", "BL",
		"ECX", "CX", "CH", "CL",
		"EDX", "DX", "DH", "DL",
		"ESI", "SI", "EDI", "DI",
		"EBP", "BP", "ESP", "SP",
		"EIP", "IP"
	};

	// If the input "r" is outside of the range of possible values
	if ( !( r >= 0 && r < (sizeof(reg) / sizeof(reg[0])) ) ) {
		return "Invalid Register";
	}

	return reg[r];
}


void CPU::toggleStart()
{

	this->start = !this->start; //add a check in CPU to check if start is toggled or not

}

void CPU::reset()
{
	this->EAX = 0;
	this->EBX = 0;
	this->ECX = 0;
	this->EDX = 0;
	this->EIP = 0;
	this->ESI = 0;
	this->EDI = 0;
	this->EBP = 0;
	this->ESP = 0x1000; // <-- back to the end of the page.
	this->eflags.value = 0;
}

/*
	switch (reg)
	{
	case registers::A:
	{
		this->A = value[1];
	} break;
	case registers::BC:
	{
		this->C = value[1];
		this->B = value[2];
	} break;
	case registers::SP:
	{
		//NOT TESTED
		//SP.low = byte 2
		*((uint8_t*)&this->SP + 1) = value[1];//get the FIRST 8 BIT (SP is 16 bit) address of the SP, add 1 (go to the next byte), derefence it, then assign
		//this->SP << value[0];
		//SP.high = byte 3
		this->SP = value[2];
	} break;
*/

void CPU::load_register_value(registers& reg, uint32_t* value)
{
	switch (reg)
	{
	case registers::EAX:
	{
		//this->EAX = (this->EAX & 0xFFFFFF00) | value[1];
		this->EAX = value[1];
	} break;
	case registers::AX:
	{
		this->AX = value[1];
	} break;
	case registers::EBX:
	{
		//this->EBX = (this->EBX & 0xFFFFFF00) | value[1];
		this->EAX = value[1];
	} break;
	case registers::ECX:
	{
		//this->ECX = (this->ECX & 0xFFFFFF00) | value[1];
		this->ECX = value[1];
	} break;
	case registers::EDX:
	{
		//this->EDX = (this->EDX & 0xFFFFFF00) | value[1];
		this->EDX = value[1];
	} break;
	case registers::ESI:
	{
		//this->ESI = (this->ESI & 0xFFFFFF00) | value[1];
		this->ESI = value[1];
	} break;
	case registers::EDI:
	{
		//this->EDI = (this->EDI & 0xFFFFFF00) | value[1];
		this->EDI = value[1];
	} break;
	case registers::ESP:
	{
		// SP is 32-bit, little-endian
		//this->ESP = (this->ESP & 0xFFFF00FF) | (value[1] << 8);
		//this->ESP = (this->ESP & 0xFF00FFFF) | (value[2] << 16);
		this->ESP = (value[1] << 8) | value[2];
	} break;
	case registers::EBP:
	{
		//this->EBP = (this->EBP & 0xFFFF00FF) | (value[2] << 8);
		this->EBP = (value[1] << 8) | value[2];
	} break;
	default:
	{
		printf("INVALID REGISTER FOR fn load_register_value");
	} break;
	}
}

