#ifndef GUI_H
#define GUI_H

#include <array>

#include "../thirdparty/imgui/imgui.h"
#include "../thirdparty/imgui_club/imgui_memory_editor.h"
#include "./cpu/CPU.h"
#include <deque>
#include <string>

using std::deque;
using std::string;

class i80386;

class MemoryViewer
{
public:
	MemoryViewer() = default;

public:
	void setup_mem(Memory* ram);
	void setup_cpu(CPU* cpu);
	void draw_memory_viewer();

	void draw_register_info();
	void draw_last_exec_opcodes();
	void draw_cpu_ctrl_buttons();
	void draw_startstop_button();
	void draw_section_details();
	void requestNewFileButton(bool isFileLoaded);
	void clearOpcodeTable();

	struct opcodeTuple
	{
		string addr;
		string opcodes;
		string disAsmInstr;
	};
	void addToOpcodesDeque(opcodeTuple opcode);


private:

	Memory* m_Memory = nullptr;
	CPU* m_Cpu = nullptr;
	MemoryEditor m_MemEditor;

	deque<opcodeTuple> last512opcodes;
};

#endif