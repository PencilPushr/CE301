#pragma once

#include "PEBase.h"
#include <vector>

namespace PE
{
    class PESectionHeaders : PEBase
    {
    public:
        PESectionHeaders(HMODULE moduleHandle);

        // Get the number of sections
        size_t GetNumberOfSections() const;

        // Get information about a specific section
        bool GetSectionHeader(size_t index, IMAGE_SECTION_HEADER& sectionHeader) const;

    private:
        std::vector<IMAGE_SECTION_HEADER> sectionHeaders;
    };
}