#pragma once

#include "PEBase.h"
#include "Section.h"
#include <vector>

#define MEMORYMAXSIZE 0xFFFF

namespace PE
{
    class PEMemoryMapper : public PEBase
    {
    public:
        static constexpr uint8_t DefaultBytes = 4;
        struct sectionTuple
        {
            //PIMAGE_SECTION_HEADER header;
            std::string header_name;
            bool isExec;
            std::vector<uint8_t> hexdata;
        };

        PEMemoryMapper(const std::string& filePath);
        ~PEMemoryMapper();

        std::vector<sectionTuple> printSectionHeaders();
        void cpySectionHeadersToArray(uint8_t* inputArray, size_t arraySize, std::vector<Section<4>>& sectionsToInsertIn);

        // Get the mapped view of the PE file
        LPVOID GetMappedView() const { return mappedView; } // Note: this is just the image base in memory. NOT TO BE CONFUSED WITH THE HANDLE, OR DISK BASE.


    private:
        HANDLE fileHandle;
        HANDLE fileMapping;
        LPVOID mappedView;
        PIMAGE_DOS_HEADER m_dosHeader;
        PIMAGE_NT_HEADERS m_ntHeader;
    protected:
        virtual LPVOID GetBaseAddress() const override { return mappedView; }
    };
}
