#include "PELoadLib.h"

namespace PE
{
    PELoadLib::PELoadLib(const std::wstring& filePath)
        : moduleHandle(NULL)
    {
        // Load the PE file into the process's address space
        moduleHandle = LoadLibraryExW(filePath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
        if (moduleHandle == NULL) {
            std::cerr << "Error loading file with LoadLibraryExW." << std::endl;
            return;
        }

        // Determine the size of the image (assuming it's a PE file)
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleHandle);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD>(moduleHandle) + dosHeader->e_lfanew);
        m_imageSize = ntHeader->OptionalHeader.SizeOfImage;
    }

    PELoadLib::~PELoadLib()
    {
        // Unload the module in the destructor
        if (moduleHandle != NULL)
            FreeLibrary(moduleHandle);
    }
}