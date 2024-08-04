#include "PESectionHeaders.h"

namespace PE
{
    PESectionHeaders::PESectionHeaders(HMODULE moduleHandle)
    {
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(moduleHandle);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<DWORD>(moduleHandle) + dosHeader->e_lfanew);

        // Get the pointer to the first section header
        PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(ntHeader);

        // Get the image size
        m_imageSize = ntHeader->OptionalHeader.SizeOfImage;

        // Iterate through all sections and store their information
        for (size_t i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
        {
            sectionHeaders.push_back(firstSection[i]);
        }
    }

    size_t PESectionHeaders::GetNumberOfSections() const
    {
        return sectionHeaders.size();
    }

    // pass in the variable you wish to acquire this will assign the IMAGE_SECTION_HEADER within the function.
    bool PESectionHeaders::GetSectionHeader(size_t index, IMAGE_SECTION_HEADER& sectionHeader) const
    {
        if (index < sectionHeaders.size())
        {
            sectionHeader = sectionHeaders[index];
            return true;
        }

        return false;
    }
}