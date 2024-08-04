#include "i80386.h"

#include "imgooey/imgooey.h"
#include "dx11/dx11.h"
#include "mem/PEMemoryMapper.h"
#include "commdlg.h"

#include <Zydis/Zydis.h>
#include <cinttypes>

//TODO: Create Reader functions to pull out all the sections of memory, e.g. if you want RAM block we can create a function to pull it all out.
struct opcodeTuple;

i80386::i80386()
{

    // Initialize Direct3D
    dx11::Init(window.hwnd);

    // Setup Dear ImGui context
    imgooey::Init(window.hwnd);

    //OR works because we can set 32 bits of an int. e.g. 0000 -> 0010 sets 1 flag, 0011 sets different flags...etc.
    imgooey::SetFlags(ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NavEnableKeyboard);

    memview->setup_mem(&this->m_memory);
    memview->setup_cpu(&this->m_cpu);
}

i80386::~i80386()
{
    imgooey::Destroy();
    dx11::Destroy();
}

void i80386::loadFileIntoMemory(const std::string& file)
{
    PE::PEMemoryMapper peMapper(file);
    std::vector<Section<DEFAULT_SECTION_BYTES>> sections;
    peMapper.cpySectionHeadersToArray(m_memory.getAddrToMemory()->data(), m_memory.getAddrToMemory()->size(), sections);
    for (int i = 0; i < sections.size(); ++i)
    {
        m_memory.insertSection(sections[i]);
    }
    /* Keeping this here for legacy sake - This will be useful to implement a more generic way to load the memory in
    * Such as using ELF format, we can still utilise this code.
    for (const auto& section : sections)
    {
        const std::string& sectionName = section.header_name;
        const std::vector<uint8_t>& sectionData = section.hexdata;
        uint32_t sectionSize = (uint32_t) sectionData.size();

        // Load the section data into the emulator's memory
        m_memory.LoadSection<DEFAULT_SECTION_BYTES>(sectionName, 0, sectionSize);
        Section<DEFAULT_SECTION_BYTES> memSection = m_memory.findSection(sectionName); 
        m_memory.DirectWriteAnyBytes<DEFAULT_SECTION_BYTES>(memSection, 0, sectionData.data(), sectionSize, m_error);
    }
    */
}

void i80386::initializeZydisDecoderFormatter(ZydisDecoder& decoder, ZydisFormatter& formatter)
{
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LEGACY_32, ZYDIS_ADDRESS_WIDTH_32);
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
}

void i80386::initTextSection()
{
    // Get reference to the sections hashmap
    const auto sections = m_memory.getAddrToSections();

    // Find the .text section
    for (const auto& section : sections)
    {
        if (section.second.isExec)
        {
            m_textSection = section.second;
            break;
        }
    }

    mp_textData = m_memory.GetRawPtr(m_textSection, 0, m_error);
    m_textSize = m_textSection.size;
}

void i80386::updateAffectedRegisters(const ZydisDecodedInstruction& instruction)
{
    // Get the instruction segments
    ZydisInstructionSegments segments;
    ZyanStatus status = ZydisGetInstructionSegments(&instruction, &segments);
    if (!ZYAN_SUCCESS(status))
    {
        // Handle error
        return;
    }

    // Iterate through the operands of the instruction
    for (uint8_t i = 0; i < instruction.operand_count; ++i)
    {
        const ZydisDecodedOperand& operand = instruction.operands[i];
        ZydisRegister reg = operand.reg.value;
        bool wasRegister = false;

        // Get the operand value based on the operand type
        uint32_t value = 0;
        switch (operand.type)
        {
        case ZYDIS_OPERAND_TYPE_REGISTER:
        {
            reg = instruction.operands[1].reg.value;
            // Ideally we just use operand.reg.value as index to our own registers as the operand value is already in the register
            // /*value = operand.reg.value;*/
            // But we have to do it the following way: get the value from the corresponding register in m_cpu
            switch (reg)
            {
            case ZYDIS_REGISTER_EAX:
                value = m_cpu.EAX;
                break;
            case ZYDIS_REGISTER_EBX:
                value = m_cpu.EBX;
                break;
            case ZYDIS_REGISTER_ECX:
                value = m_cpu.ECX;
                break;
            case ZYDIS_REGISTER_EDX:
                value = m_cpu.EDX;
                break;
            case ZYDIS_REGISTER_ESI:
                value = m_cpu.ESI;
                break;
            case ZYDIS_REGISTER_EDI:
                value = m_cpu.EDI;
                break;
            case ZYDIS_REGISTER_EBP:
                value = m_cpu.EBP;
                break;
            case ZYDIS_REGISTER_ESP:
                value = m_cpu.ESP;
                break;
            default:
                // Unknown or unsupported register
                break;
            }
            reg = operand.reg.value;
            break;
        }
        case ZYDIS_OPERAND_TYPE_IMMEDIATE:
        {
            // Get the immediate value directly from the operand
            value = operand.imm.value.u;
            break;
        }
        case ZYDIS_OPERAND_TYPE_MEMORY:
        {
            // Get the memory address from the operand
            uint32_t address = operand.mem.disp.value;
            // Read the value from memory at the given address
            value = m_memory.DirectReadBytes(m_textSection, m_disassemblyCursor + address, m_error);
            break;
        }
        default:
        {
            // Unsupported operand type
            continue;
        }
        }


        // Update the corresponding register based on the ID
        switch (reg)
        {
        case ZYDIS_REGISTER_EAX:
            m_cpu.EAX = value;
            break;
        case ZYDIS_REGISTER_EBX:
            m_cpu.EBX = value;
            break;
        case ZYDIS_REGISTER_ECX:
            m_cpu.ECX = value;
            break;
        case ZYDIS_REGISTER_EDX:
            m_cpu.EDX = value;
            break;
        case ZYDIS_REGISTER_ESI:
            m_cpu.ESI = value;
            break;
        case ZYDIS_REGISTER_EDI:
            m_cpu.EDI = value;
            break;
        case ZYDIS_REGISTER_EBP:
            m_cpu.EBP = value;
            break;
        case ZYDIS_REGISTER_ESP:
            m_cpu.ESP = value;
            break;
        default:
            // Unknown or unsupported register
            break;
        }

    }
}

void i80386::disassembleTextSection()
{

    if (m_textSection.size == 0)
    {
        // .text section not found or is empty
        printf("[MemoryViewer] textSection was not found or is empty.\n\n");
        return;
    }

    // Initialize the disassembly cursor and flag
    m_disassemblyCursor = m_textSection.startAddress;
    m_disassemblyDone = false;

    // Disassemble the entire .text section
    while (m_disassemblyCursor < m_textSection.startAddress + m_textSection.virtualSize)
    {
        disassembleTextSectionSingleStep();
    }

    m_disassemblyDone = true;
}


bool i80386::disassembleTextSectionSingleStep()
{

    if (m_disassemblyCursor < m_textSection.startAddress + m_textSection.virtualSize)
    {
        uint8_t* p_buffer = reinterpret_cast<uint8_t*>(mp_textData + (m_disassemblyCursor - m_textSection.startAddress));

        ZydisDecodedInstruction instruction;
        ZyanStatus status = ZydisDecoderDecodeBuffer(
            &m_decoder, 
            p_buffer,
            m_textSize - (m_disassemblyCursor - m_textSection.startAddress),
            &instruction
        );

        if ( ! (ZYAN_SUCCESS(status)) )
        {
            return false;
        }

  

        // Update EIP to the current SUCCESSFULLY decoded instruction
        m_cpu.EIP = m_disassemblyCursor;
        updateAffectedRegisters(instruction);

        char buffer[256];
        ZydisFormatterFormatInstruction(&m_formatter, &instruction, buffer, sizeof(buffer), m_disassemblyCursor);
        std::string disassembledInstruction(buffer);
        
        // Get the raw bytes of the instruction
        std::stringstream rawBytesStream;
        for (size_t i = 0; i < instruction.length; ++i)
        {
            rawBytesStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*(p_buffer + i));
        }
        std::string rawBytes = rawBytesStream.str();

        // Create a stringstream to format the runtime address
        std::stringstream addressStream;
        addressStream << "0x" << std::hex << std::setw(4) << std::setfill('0') << m_disassemblyCursor;
        std::string formattedAddress = addressStream.str();

        // Add the disassembled instruction to the last512opcodes deque
        memview->addToOpcodesDeque( { formattedAddress, rawBytes, disassembledInstruction } );

        // Update the CPU flags
        m_cpu.setFlags(instruction);

        m_disassemblyCursor += instruction.length;

        return true;
    }
}

void i80386::resetDisassemblyPosition()
{
    m_disassemblyCursor = m_textSection.startAddress;
    m_disassemblyDone = false;
    memview->clearOpcodeTable();
}

std::string i80386::requestFileFromUser()
{
    static char filePath[256] = "";
    std::string selectedFile;

    ImGui::Begin("File Selector");
    ImGui::InputText("File Path", filePath, sizeof(filePath));
    if (ImGui::Button("Load File"))
    {
        selectedFile = filePath;
    }
    ImGui::End();

    return selectedFile;
}

std::string openFileDialog(const char* filter = "All Files (*.*)\0*.*\0")
{
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";

    if (GetOpenFileNameA(&ofn))
        return fileName;
    else
        return "";
}

void i80386::drawDisasmCtrls()
{
    ImGui::Begin("Disasm ctrls");
    if (ImGui::Button("Disassemble Entire Section"))
    {
        this->disassembleTextSection();
    }

    if (ImGui::Button("Single Step Disassembly"))
    {
        this->disassembleTextSectionSingleStep();
    }

    if (ImGui::Button("Reset Disassembly Position"))
    {
        this->resetDisassemblyPosition();

        //
        // Reset the CPU & registers
        // If esp is allocated, free it first to
        // stop the memory leak
        //
        this->m_cpu.reset();
    }
    /*
    if (ImGui::Button("Continuous Disassembly"))
    {
        //this->continuousDisassembly();
    }
    */
    ImGui::End();
}

void i80386::Run()
{
    this->m_cpu.toggleStart();
    bool isFileLoaded = false;


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        pw32_HandleMessages(done);
        if (done)
            break;
        dx11::ClearScreen(0.45f, 0.55f, 0.60f, 1.00f);
        imgooey::Begin();

        // Kind of bad code, doing a check every loop seems idiotic.
        if (!isFileLoaded)
        {
            //std::string selectedFile = requestFileFromUser();
            std::string selectedFile = openFileDialog();
            if (!selectedFile.empty())
            {
                loadFileIntoMemory(selectedFile); // This will check if the file is PE or not
                initTextSection();
                initializeZydisDecoderFormatter(m_decoder, m_formatter);
                disassembleTextSection();
                isFileLoaded = true;
            }
            else
            {
                printf("\n[ERROR] No file selected, exiting.\n\n");
                done = true;
            }
        }
        else
        {

            // Draw Here
            memview->draw_memory_viewer();
            memview->draw_last_exec_opcodes();
            memview->draw_section_details();
            memview->draw_register_info();
            drawDisasmCtrls();
        }

        imgooey::End();
        dx11::Render();
    }

}

