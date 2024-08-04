#pragma once

#include "PEBase.h"

namespace PE
{
    class PELoadLib : public PEBase
    {
    public:
        PELoadLib(const std::wstring& filePath);
        ~PELoadLib();

        // Get the base address of the loaded module
        HMODULE GetModuleHandle() const { return moduleHandle; }

    private:
        HMODULE moduleHandle;

    protected:
        virtual LPVOID GetBaseAddress() const override { return moduleHandle; }
    };
}