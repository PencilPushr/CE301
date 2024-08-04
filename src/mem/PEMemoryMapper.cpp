#include "PEMemoryMapper.h"
#include <winnt.h>
#include <ranges>
#undef min

#define PRINT

namespace PE
{
    PEMemoryMapper::PEMemoryMapper(const std::string& filePath)
        : fileHandle(INVALID_HANDLE_VALUE), fileMapping(NULL), mappedView(NULL), m_dosHeader(NULL), m_ntHeader(NULL)
    {
        // Open the file
        fileHandle = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Error opening file." << std::endl;
            return;
        }

        // Get the file size
        DWORD fileSize = GetFileSize(fileHandle, NULL);

        // Create a file mapping object
        fileMapping = CreateFileMapping(fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
        if (fileMapping == NULL) {
            std::cerr << "Error creating file mapping." << std::endl;
            CloseHandle(fileHandle);
            return;
        }

        // Map the file into memory
        mappedView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
        if (mappedView == NULL) {
            std::cerr << "Error mapping view of file." << std::endl;
            CloseHandle(fileMapping);
            CloseHandle(fileHandle);
            return;
        }

        // Determine the size of the image (assuming it's a PE file)
        PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(mappedView);
        PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((UINT_PTR)mappedView + dosHeader->e_lfanew);
        PEBase::m_imageSize = ntHeader->OptionalHeader.SizeOfImage;
        m_dosHeader = dosHeader;
        m_ntHeader = ntHeader;
    }

    std::vector<PEMemoryMapper::sectionTuple> PEMemoryMapper::printSectionHeaders()
    {
        // Create vector to store the header with it's corresponding data
        std::vector<sectionTuple> headersAndData;

        // Get the pointer to the first section header
        PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(m_ntHeader);

        for (size_t i = 0; i < m_ntHeader->FileHeader.NumberOfSections; ++i)
        {
            sectionTuple currentTuple;

            // Collect the header name
            currentTuple.header_name = (const char*)firstSection[i].Name;

            // Get base address of pointer
            //uint8_t* rawDataPtr = (uint8_t*)((UINT_PTR)m_dosHeader + firstSection[i].PointerToRawData);
            printf("Section name: %s --- Virtual address: %x --- size of rawdata: %d\n", (const char*)firstSection[i].Name, firstSection[i].VirtualAddress, firstSection[i].SizeOfRawData);

            // Iterate over the raw data
            for (DWORD j = 0; j < firstSection[i].SizeOfRawData; j++)
            {
                currentTuple.hexdata.push_back(
                    ((uint8_t*)((UINT_PTR)m_dosHeader + firstSection[i].PointerToRawData))[j] // weird ass casting just to index this properly
                );
            }

            headersAndData.push_back(currentTuple);

#ifdef PRINT
            // Print relevant data
            printf("Section name: %s\nSection size: %d\nSection data: ", currentTuple.header_name.c_str(), currentTuple.hexdata.size());
            for (uint8_t byte : currentTuple.hexdata)
            {
                printf("%02X ", byte); // Assuming you want to print the bytes as hexadecimal
            }
            printf("\n\n");
#endif
        }

        return headersAndData;
    }

    void PEMemoryMapper::cpySectionHeadersToArray(uint8_t* inputArray, size_t arraySize, std::vector<Section<DefaultBytes>>& sectionsToInsertIn)
    {
        // Get the pointer to the first section header
        PIMAGE_SECTION_HEADER firstSection = IMAGE_FIRST_SECTION(m_ntHeader);

        uint32_t initialPageBoundary = firstSection[0].VirtualAddress;

        for (size_t i = 0; i < m_ntHeader->FileHeader.NumberOfSections; ++i)
        {
            PIMAGE_SECTION_HEADER CurrentSection = &firstSection[i];
            
            // Assuming 4-byte addresses (change the template parameter if needed)
            Section<DefaultBytes> section;

            // Collect the section name
            section.name = (const char*)CurrentSection->Name;

            // Check if the flag IMAGE_SCN_CNT_CODE is triggered -> meaning this is the executable code
            // This is because it is possible to have a .textbss section where the code is not inherently executable but is part of the .text
            section.isExec = CurrentSection->Characteristics & IMAGE_SCN_CNT_CODE;

            //IMAGE_SCN_CNT_UNINITIALIZED_DATA
            //IMAGE_SCN_MEM_DISCARDABLE
            //IMAGE_SCN_LNK_NRELOC_OVFL

            const auto rebasedAddress = CurrentSection->VirtualAddress - 0x1000;
            //const auto rebasedAddress = CurrentSection->VirtualAddress - initialPageBoundary;

            // Store the aligned virtual address and aligned size in the section
            section.startAddress = static_cast<typename ByteSize<4>::Type>(rebasedAddress);
            section.size = static_cast<typename ByteSize<4>::Type>(CurrentSection->SizeOfRawData);
            section.virtualSize = static_cast<typename ByteSize<4>::Type>(CurrentSection->Misc.VirtualSize);
            
            // Insert the current section into the vector
            sectionsToInsertIn.emplace_back(section);

            // Insert memory into the inputArray at the correct offsets.
            memcpy(inputArray + rebasedAddress, (uint8_t*)((UINT_PTR)m_dosHeader + CurrentSection->PointerToRawData), CurrentSection->SizeOfRawData);

#ifdef PRINT
            // Print relevant data
            printf("Section name: %s\nSection Address: %x\nSection virt size: %d\nSection size: %d\nSection data: ",
                section.name.c_str(),
                section.startAddress,
                section.virtualSize,
                CurrentSection->SizeOfRawData
            );
            for (DWORD j = 0; j < CurrentSection->SizeOfRawData; j++)
            {
                printf("%02X ", inputArray[rebasedAddress + j]); // Assuming you want to print the bytes as hexadecimal
            }
            printf("\n\n");
#endif
        }

#ifdef PRINT_ARR
        printf("inputArray\n");
        for (DWORD j = 0; j < 0x3000; j++)
        {
            printf("%02X ", inputArray[j]); // Assuming you want to print the bytes as hexadecimal
        }
        printf("\n\n");
#endif
    }

    PEMemoryMapper::~PEMemoryMapper()
    {

        // Unmap the view and close handles in the destructor
        if (mappedView != NULL)
            UnmapViewOfFile(mappedView);

        if (fileMapping != NULL)
            CloseHandle(fileMapping);

        if (fileHandle != INVALID_HANDLE_VALUE)
            CloseHandle(fileHandle);
    }
}