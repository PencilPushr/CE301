#include "HexBin.h"
#include <bitset>
#include <istream>
#include <sstream>
#include <fstream>
#include <ostream>
#include <vector>
#include <string>
#include <Windows.h>

std::string hexBytetoBin(const std::string& hex)
{
    unsigned int num = std::stoul(hex, nullptr, 16);
    return std::bitset<8>(num).to_string();
}

std::string toBin(const std::string& hex)
{
    if (hex.size() < 2 || hex.size() % 2 != 0)
        return "-1";
    std::string out;
    for (int i = 0; i < hex.size(); i += 2)
    {
        std::string byte;
        std::memcpy(&byte[0], &hex[i], 2);
        out += hexBytetoBin(byte);
    }
    return out;
}
//gets rid of the colons
std::string deweirdify(const std::string& hex)
{
    std::string out;
    for (char c : hex)
        if (c != ':' && c != '\n')
            out += c;
    return out;
}

std::string constructBin(const char* filename)
{
    std::ifstream stream{ filename };
    if (!stream.is_open())
        return "-1";
    std::stringstream filecontentsbuf;
    filecontentsbuf << stream.rdbuf();
    std::string contents = deweirdify(filecontentsbuf.str());
    return toBin(contents);
}

std::string constructHex(const char* filename)
{
    std::ifstream stream{ filename };
    if (!stream.is_open())
        return "-1";
    std::stringstream filecontentsbuf;
    filecontentsbuf << stream.rdbuf();
    std::string contents = deweirdify(filecontentsbuf.str());
    return contents;

}


std::string PE_constructHex(const char* path)
{
    std::fstream stream(path, std::ios::out | std::ios::binary);

    if (!stream.is_open())
    {
        return "-1";
    }

    std::vector<unsigned char> file = { };

    stream.write((char*)file.data(), file.size());

    std::vector<unsigned char> data_out = { };

    const auto dos = PIMAGE_DOS_HEADER(file.data());

    if (!dos || dos->e_magic != 0x5A4D) // Not equal to MZ file (look up MZ DOS FILE STRUCTURE), otherwise invalid file
    {
        return "-1";
    }

    const auto nt = PIMAGE_NT_HEADERS(file.data() + dos->e_lfanew); // Base of file + rva (to start of nt headers)

    if (!nt)
    {
        return "-1";
    }

    data_out.resize(nt->OptionalHeader.SizeOfImage);

    auto sect_header = IMAGE_FIRST_SECTION(nt); // Address of the first nt section header

    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) // Loop through all the sections, 
    {
        auto sect = sect_header[i];

        memcpy(data_out.data() + sect.VirtualAddress, file.data() + sect.PointerToRawData, sect.SizeOfRawData);
    }

    //const auto contents = deweirdify(file);

    return (const char*)file.data();
}

std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}
