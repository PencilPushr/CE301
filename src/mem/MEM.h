#pragma once

#include <array>
#include <string>
#include "Error.h"
#include <iostream>
#include <bitset>
#include <vector>
#include <unordered_map>

#include "../util/Util.h"
//#include "ByteSize.h" //Section.h already includes ByteSize so I'm just avoiding any include issues
#include "Section.h"

#define MEMORYSTART 0x0000
#define MEMORYMAXSIZE 0xFFFF
#define DEFAULT_SECTION_BYTES 4

template<uint8_t P> constexpr bool IsByteAligned()
{
    return 8 % P == 0;
}

class Memory
{
public:
    Memory();
    ~Memory();

    using Ptr = uint32_t;
    static constexpr uint8_t DefaultBytes = 4;
    static constexpr uint32_t TotalAdrSpaceLwrBnd = 0x0000;
    static constexpr uint32_t TotalAdrSpaceUprBnd = 0xFFFF;


    template<typename T>
    void LoadSection(const std::string& name, T startAddress, T size)
    {
        m_Sections[name] = Section<T>{ name, startAddress, size };
    }


    template<uint8_t Bytes = DefaultBytes, typename SectionType>
    typename ByteSize<Bytes>::Type DirectReadBytes(SectionType section, Ptr offset, ErrorCode& e)
    {
        if (!AddressableRangeCheck(section.startAddress + offset + Bytes, e))
        {
            e.flag |= (int)ErrorFlags::INVALID_READ | (int)ErrorFlags::INVALID_ARGUMENT;
            SetMemSectionFlag(section, e);
            return 0;
        }
        //CheckMemUpper(section.startAddress + section.size, section.startAddress + offset + Bytes, e);
        CheckMemBounds(section, offset, Bytes, e);
        if (e.flag & (int)ErrorFlags::MEM_OUT_OF_BOUND)
        {
            e.flag |= (int)ErrorFlags::INVALID_READ;
            SetMemSectionFlag(section, e);
            SetDebugECMsg(e, "Memory read through DirectReadByte out of bound.\n"
                "Read attempt at offset " + toHexStr(offset) +
                ".\nNote that valid memory locations for " + section.name + " are 0x00 to " +
                toHexStr(section.size));
            return 0;
        }
        typename ByteSize<Bytes>::Type value = JoinBytes<Bytes>(section.startAddress + offset);
        SetDebugECMsg(e, "Operation succeeded");
        return value;
    }

    template<typename SectionType>
    uint8_t* GetRawPtr(SectionType section, Ptr offset, ErrorCode& e)
    {
        if (!AddressableRangeCheck(section.startAddress + offset, e))
        {
            e.flag |= (int)ErrorFlags::INVALID_READ | (int)ErrorFlags::INVALID_ARGUMENT;
            SetMemSectionFlag(section, e);
            return nullptr;
        }
        //CheckMemUpper(section.startAddress + section.size, section.startAddress + offset, e);
        CheckMemBounds(section, offset, 1, e); 
        if (e.flag & (int)ErrorFlags::MEM_OUT_OF_BOUND)
        {
            e.flag |= (int)ErrorFlags::INVALID_READ;
            SetMemSectionFlag(section, e);
            SetDebugECMsg(e, "Memory read through GetRawPtr out of bound.\n"
                "Read attempt at offset " + toHexStr(offset) +
                ".\nNote that valid memory locations for " + section.name + " are 0x00 to " +
                toHexStr(section.size));
            return nullptr;
        }
        SetDebugECMsg(e, "Operation succeeded");
        return m_Memory.data() + section.startAddress + offset;
    }

    template<uint8_t PtrBytes = DefaultBytes, uint8_t DataBytes = DefaultBytes, typename PtrSectionType, typename DataSectionType>
    typename ByteSize<DataBytes>::Type IndirectReadBytes(PtrSectionType ptrSection, DataSectionType dataSection, Ptr offset, ErrorCode& e)
    {
        typename ByteSize<PtrBytes>::Type adr = DirectReadBytes<PtrBytes>(ptrSection, offset, e);
        typename ByteSize<DataBytes>::Type data = DirectReadBytes<DataBytes>(dataSection, adr, e);
        return data;
    }

    template<uint8_t Bytes = DefaultBytes, typename SectionType>
    void DirectWriteBytes(SectionType section, Ptr offset, typename ByteSize<Bytes>::Type data, ErrorCode& e)
    {
        if (!AddressableRangeCheck(section.startAddress + offset, e))
        {
            e.flag |= (int)ErrorFlags::INVALID_READ | (int)ErrorFlags::INVALID_ARGUMENT;
            SetMemSectionFlag(section, e);
            return;
        }
        //CheckMemUpper(section.startAddress + section.size, section.startAddress + offset, e);
        CheckMemBounds(section, offset, Bytes, e);
        if (e.flag & (int)ErrorFlags::MEM_OUT_OF_BOUND)
        {
            e.flag |= (int)ErrorFlags::INVALID_WRITE;
            SetMemSectionFlag(section, e);
            SetDebugECMsg(e, "Memory write through DirectWriteByte out of bound.\n"
                "Write attempt at offset " + toHexStr(offset) +
                ".\nNote that valid memory locations for " + section.name + " are 0x00 to " +
                toHexStr(section.size));
            return;
        }
        memcpy(m_Memory.data() + section.startAddress + offset, &data, Bytes);
        SetDebugECMsg(e, "Operation succeeded");
    }

    template<typename SectionType>
    void DirectWriteAnyBytes(SectionType section, Ptr offset, uint8_t* data, Ptr bytes, ErrorCode& e)
    {
        if (!AddressableRangeCheck(section.startAddress + offset + bytes, e))
        {
            e.flag |= (int)ErrorFlags::INVALID_READ | (int)ErrorFlags::INVALID_ARGUMENT;
            SetMemSectionFlag(section, e);
            return;
        }
        //CheckMemUpper(section.startAddress + section.size, section.startAddress + offset + bytes, e);
        CheckMemBounds(section, offset, bytes, e);
        if (e.flag & (int)ErrorFlags::MEM_OUT_OF_BOUND)
        {
            e.flag |= (int)ErrorFlags::INVALID_WRITE;
            SetMemSectionFlag(section, e);
            SetDebugECMsg(e, "Memory write through DirectWriteAnyBytes out of bound.\n"
                "Write attempt at offset " + toHexStr(offset) +
                ".\nNote that valid memory locations for " + section.name + " are 0x00 to " +
                toHexStr(section.size));
            return;
        }
        memcpy(m_Memory.data() + section.startAddress + offset, data, bytes);
        SetDebugECMsg(e, "Operation succeeded");
    }

    template<uint8_t Bytes = DefaultBytes, bool Reverse = true>
    typename ByteSize<Bytes>::Type JoinBytes(Ptr offset)
    {
        if constexpr (Bytes == 1)
        {
            return m_Memory[offset];
        }

        typename ByteSize<Bytes>::Type index = 0;
        if constexpr (Reverse)
        {
            uint8_t i = Bytes;
            do
            {
                index = (index << (8 * (Bytes / 2))) | m_Memory[offset + i - 1];
                i--;
            } while (i > 0);
            return index;
        }
        else
        {
            uint8_t i = 0;
            do
            {
                if constexpr (Bytes == 1)
                {
                    index = m_Memory[offset];
                    break;
                }
                index = (index << (8 * (Bytes / 2))) | m_Memory[offset + i];
                i++;
            } while (i < Bytes);
            return index;
        }
    }

    void Reset();

private:
    void CheckMemUpper(uint16_t min, uint16_t value, ErrorCode& e);
    void CheckMemLower(uint16_t max, uint16_t value, ErrorCode& e);

    template<typename SectionType>
    void CheckMemBounds(SectionType section, uint32_t offset, uint32_t size, ErrorCode& e)
    {
        uint32_t startAddress = section.startAddress;
        uint32_t endAddress = section.startAddress + section.size;

        if (offset < startAddress || offset + size > endAddress)
        {
            e.flag |= (int)ErrorFlags::MEM_OUT_OF_BOUND;
            SetDebugECMsg(e,
                "Memory access out of bounds. Accessed range: [" + toHexStr(offset) +
                ", " + toHexStr(offset + size) + "), Section range: [" +
                toHexStr(startAddress) + ", " + toHexStr(endAddress) + ")");
            return;
        }
    }

private:
    bool AddressableRangeCheck(int p, ErrorCode& e);

public:
    // Upr and LwrBnd functions also double as mutators :)
    template<typename SectionType>
    auto GetLwrBnd(SectionType section)
    {
        return section.startAddress;
    }

    // Doubles as a mutator
    template<typename SectionType>
    auto GetUprBnd(SectionType section)
    {
        return section.startAddress + section.size;
    }

    template<typename SectionType>
    const char* GetName(SectionType section)
    {
        return section.name.c_str();
    }

    template<typename SectionType>
    void SetMemSectionFlag(SectionType section, ErrorCode& e)
    {
        e.flag |= (int)ErrorFlags::MEMORY_SECTION_CUSTOM;
    }

    std::array<uint8_t, MEMORYMAXSIZE>* getAddrToMemory()
    {
        return &m_Memory;
    }

    std::unordered_map<std::string, Section<DEFAULT_SECTION_BYTES>>& getAddrToSections()
    {
        return m_Sections;
    }

    std::unordered_map<std::string, Section<DEFAULT_SECTION_BYTES>> getSections()
    {
        return m_Sections;
    }

    Section<DEFAULT_SECTION_BYTES> getSection(const std::string& sectionName)
    {
        return m_Sections[sectionName];
    }

    void insertSection(Section<DEFAULT_SECTION_BYTES> sectionInput)
    {
        m_Sections[sectionInput.name] = sectionInput;
    }
     

private:
    std::array<uint8_t, MEMORYMAXSIZE> m_Memory = { 0 };
    std::unordered_map<std::string, Section<DEFAULT_SECTION_BYTES>> m_Sections;
};