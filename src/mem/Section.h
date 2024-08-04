#pragma once

#include "ByteSize.h"

template<uint8_t Bytes>
struct Section
{
    std::string name;
    bool isExec; // We can check if it's executable see -> IMAGE_SCN_CNT_CODE in 
    typename ByteSize<Bytes>::Type startAddress;
    typename ByteSize<Bytes>::Type size; // Size of the section
    typename ByteSize<Bytes>::Type virtualSize; // Size of the data IN the section
};

