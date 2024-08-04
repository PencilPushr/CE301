#include "MemoryViewer.h"

#include <memory>
#include <Zydis/Zydis.h>
#include <windows.h>
#include "../thirdparty/imgui/imgui.h"
#include "mem/MEM.h"

bool CPU::start;

//incase we switch back to std::array<uint8_t, 0xFFFF>* memoryToView
//then just going to keep this placeholder: void MemoryViewer::setup(std::array<uint8_t, 0xFFFF>* memoryToView)
void MemoryViewer::setup_mem(Memory* memoryToView)
{
	auto style = &ImGui::GetStyle();

	style->AntiAliasedFill = 0;
	style->AntiAliasedLines = 0;
	style->AntiAliasedLinesUseTex = 0;

	//style->FramePadding = ImVec2(20, 20);

	style->WindowMinSize = { 24, 24 };
	style->WindowPadding = { 0, 0 };
	style->WindowRounding = 3;
	style->WindowBorderSize = 0;

	style->PopupRounding = 3.f;
	style->PopupBorderSize = 0.f;

	style->ScrollbarSize = 7;

	style->GrabMinSize = 1;

	style->Colors[ImGuiCol_WindowBg] = ImColor{ 35, 35, 35 };

	ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 14);

    m_Memory = memoryToView;
}

void MemoryViewer::setup_cpu(CPU* cpu)
{
    this->m_Cpu = cpu;
}

void MemoryViewer::draw_memory_viewer()
{
	m_MemEditor.DrawWindow("RAM", m_Memory->getAddrToMemory()->data(), 0xFFFF);
}

void MemoryViewer::draw_register_info()
{
    std::string instr_info = "Current instruction: " + std::to_string(m_Cpu->EIP) + "\nNext Byte:\n"; // Need to get cpu->get_next_instruction()

    std::string reg_info = "Registers:\n";
    reg_info += "EAX: " + std::to_string(m_Cpu->EAX) + "\n";
    reg_info += "EBX: " + std::to_string(m_Cpu->EBX) + "\n";
    reg_info += "ECX: " + std::to_string(m_Cpu->ECX) + "\n";
    reg_info += "EDX: " + std::to_string(m_Cpu->EDX) + "\n";
    reg_info += "ESI: " + std::to_string(m_Cpu->ESI) + "\n";
    reg_info += "EDI: " + std::to_string(m_Cpu->EDI) + "\n";
    reg_info += "EBP: " + std::to_string(m_Cpu->EBP) + "\n";
    reg_info += "ESP: " + std::to_string(m_Cpu->ESP) + "\n";

    //std::string seg_reg_info = "Segment Registers:\n";
    //seg_reg_info += "CS: " + std::to_string(m_Cpu->CS) + "\n";
    //seg_reg_info += "DS: " + std::to_string(m_Cpu->DS) + "\n";
    //seg_reg_info += "SS: " + std::to_string(m_Cpu->SS) + "\n";
    //seg_reg_info += "ES: " + std::to_string(m_Cpu->ES) + "\n";
    //seg_reg_info += "FS: " + std::to_string(m_Cpu->FS) + "\n";
    //seg_reg_info += "GS: " + std::to_string(m_Cpu->GS) + "\n";

    std::string flag_info = "\nFlags:\n";
    flag_info += "CF: " + std::to_string(m_Cpu->eflags.CF) + "\n";
    flag_info += "PF: " + std::to_string(m_Cpu->eflags.PF) + "\n";
    flag_info += "AF: " + std::to_string(m_Cpu->eflags.AF) + "\n";
    flag_info += "ZF: " + std::to_string(m_Cpu->eflags.ZF) + "\n";
    flag_info += "SF: " + std::to_string(m_Cpu->eflags.SF) + "\n";
    flag_info += "TF: " + std::to_string(m_Cpu->eflags.TF) + "\n";
    flag_info += "IF: " + std::to_string(m_Cpu->eflags.IF) + "\n";
    flag_info += "DF: " + std::to_string(m_Cpu->eflags.DF) + "\n";
    flag_info += "OF: " + std::to_string(m_Cpu->eflags.OF) + "\n";

    std::string extra_regs_info = "\nEIP/PC: " + std::to_string(m_Cpu->EIP) + "\n";

    auto to_print = instr_info + reg_info + /*seg_reg_info + */ flag_info + extra_regs_info;

    ImGui::Begin("CPU Info");
    ImGui::Text("%s", to_print.c_str());
    draw_cpu_ctrl_buttons();
    draw_startstop_button();
    ImGui::End();
}

void MemoryViewer::draw_last_exec_opcodes()
{

    if (ImGui::BeginTable("Last Instructions", 3))
    {
        for (size_t i = 0; i < last512opcodes.size(); i++)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(last512opcodes.at(i).addr.c_str());
            ImGui::TableNextColumn();
            ImGui::Text(last512opcodes.at(i).opcodes.c_str());
            ImGui::TableNextColumn();
            ImGui::Text(last512opcodes.at(i).disAsmInstr.c_str());
        }
    }
	
	ImGui::EndTable();
}


void MemoryViewer::draw_cpu_ctrl_buttons()
{
    if (ImGui::Button("Set CPU Clock in MHz"))
    {
        ImGui::OpenPopup("CPU Speed");
    }

    if (ImGui::BeginPopupModal("CPU Speed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Center-align the text
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Enter CPU clock speed in MHZ").x) * 0.5f);
        ImGui::Text("Enter CPU clock speed in MHZ");
        ImGui::Separator();

        static char inputBuffer[32] = "Enter Speed"; // Initial placeholder text
        static bool isEditing = false;

        if (isEditing && strcmp(inputBuffer, "Enter Speed") == 0)
        {
            // Clear the initial placeholder text when user starts typing
            inputBuffer[0] = '\0';
        }

        // ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("SpeedInput").x * 0.5f);
        isEditing = ImGui::InputText("##SpeedInput", inputBuffer, sizeof(inputBuffer) / sizeof(inputBuffer[0]),
            ImGuiInputTextFlags_EnterReturnsTrue);

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            // Handle the OK button click, e.g., store the inputBuffer content
            // and close the popup
            // Example: speedValue = inputBuffer;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            // Handle the Cancel button click if needed and close the popup
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }


}

void MemoryViewer::draw_section_details()
{
    ImGui::Begin("Section Details");

    // Get the sections from the Memory class
    const auto& sections = m_Memory->getSections();

    // Display the section details
    for (const auto& section : sections)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f)); // Orange color
        ImGui::Text("Section Name: %s", section.second.name.c_str());
        ImGui::PopStyleColor(); // Restore the default color

        ImGui::Text("Is Executable: %s", section.second.isExec ? "Yes" : "No");
        ImGui::Text("Start Address: 0x%04X", section.second.startAddress);
        ImGui::Text("Size: %u bytes", section.second.size);
        ImGui::Separator();
    }

    ImGui::End();
}

void MemoryViewer::draw_startstop_button()
{
    //TODO: Add display to tell if the program is currently paused or not.
    if (ImGui::Button("Start/Stop"))
    {
        m_Cpu->toggleStart(); //add a check in CPU to check if start is toggled or not
    }
}


void MemoryViewer::requestNewFileButton(bool isFileLoaded)
{
    isFileLoaded = false;
}

void MemoryViewer::clearOpcodeTable()
{
    if (ImGui::BeginTable("Last Instructions", 3))
    {
        // Clear the table
        ImGui::EndTable();
    }
}

void MemoryViewer::addToOpcodesDeque(opcodeTuple opcode)
{
    if (last512opcodes.size() >= 512)
    {
        last512opcodes.pop_front();
    }
    last512opcodes.push_back(opcode);
}
