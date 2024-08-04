#pragma once

#include <cstdint>

template<uint8_t> struct ByteSize;
template<> struct ByteSize<1> { using Type = uint8_t; };
template<> struct ByteSize<2> { using Type = uint16_t; };
template<> struct ByteSize<4> { using Type = uint32_t; };
template<> struct ByteSize<8> { using Type = uint64_t; };