#pragma once
#include <unordered_map>
#include <string>

enum registers : int
{
	EAX = 0,
	AX = 1,
	AH = 2,
	AL = 3,

	EBX = 4,
	BX = 5,
	BH = 6,
	BL = 7,

	ECX = 8,
	CX = 9,
	CH = 10,
	CL = 11,

	EDX = 12,
	DX = 13,
	DH = 14,
	DL = 15,

	// Index registers
	ESI = 16,
	SI = 17,

	EDI = 18,
	DI = 19,

	EBP = 20,
	BP = 21,

	ESP = 22,
	SP = 23,

	//IP/EIP or PC depends on the architecture for implementation. Intel it's IP/EIP depending on 16-32 bit.
	// Changed: adding EPC/EIP and PC/IP for Extended versions
	EIP = 24,
	IP = 25

};

static std::unordered_map<int, std::string> reg_int_to_string
{
	//GPR
	{0, "EAX"},
	{1, "AX"},
	{2, "AH"},
	{3, "AL"},

	{4, "EBX"},
	{5, "BX"},
	{6, "BH"},
	{7, "BL"},

	{8, "ECX"},
	{9, "CX"},
	{10, "CH"},
	{11, "CL"},

	{12, "EDX"},
	{13, "DX"},
	{14, "DH"},
	{15, "DL"},

	// Index registers
	{16, "ESI"},
	{17, "SI"},

	{18, "EDI"},
	{19, "DI"},

	{20, "EBP"},
	{21, "BP"},

	{22, "ESP"},
	{23, "SP"},

	{24, "EIP"},
	{25, "IP"},
};