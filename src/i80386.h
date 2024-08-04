#pragma once
#include <memory>
#include <array>
#include "MemoryViewer.h"
#include "./mem/MEM.h"
#include "Platformwin/PlatformWin32.h"
#include "../util/Util.h"
#include "../util/HexBin.h"
#include "./cpu/CPU.h"
#include "Error.h"

//i80386 includes all the GUI and the machine we will be i80386. 
//It contains the interface that the user will be interacting/viewing with.
//Two objects of the i80386 class will and should never be able to interact with each other.

class i80386
{
public:
    i80386();
    ~i80386();
    void loadFileIntoMemory(const std::string& file);
    void updateAffectedRegisters(const ZydisDecodedInstruction& instruction);
    void disassembleTextSection();
    bool disassembleTextSectionSingleStep();
    void resetDisassemblyPosition();
    void drawDisasmCtrls();
    std::string requestFileFromUser();

    void Run();

private:
    // Main components
    Memory m_memory;
    CPU m_cpu;
    ErrorCode m_error;

    // Utility
    ZyanU64 m_disassemblyCursor;
    bool m_disassemblyDone;
    ZydisDecoder m_decoder;
    ZydisFormatter m_formatter;

    // Data related to the textSection in the memoryclass
    Section<DEFAULT_SECTION_BYTES> m_textSection;
    uint8_t* mp_textData = nullptr;
    uint32_t m_textSize;
    
    // Forward declarations of helper functions
    std::unique_ptr<MemoryViewer> memview{ new MemoryViewer() };
    // Create Simulation window
    PlatformWin32 window{ L"i386_emulator", 100, 100, 600, 600 };

    void initializeZydisDecoderFormatter(ZydisDecoder& decoder, ZydisFormatter& formatter);
    void initTextSection();
};


