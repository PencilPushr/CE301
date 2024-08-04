#include "Util.h"
#include <format>
#include <iostream>
#include <fstream>
#include <vector>

std::string toUpper(const std::string& input)
{
	std::string result = input;
	for (char& c : result) {
		c = std::toupper(c);
	}
	return result;
}

std::string toHexStr(uint16_t input)
{
	return "0x" + toUpper(std::format("{:x}", input));
}

int numberOfSetBits(uint32_t i)
{
	// Java: use int, and use >>> instead of >>. Or use Integer.bitCount()
	// C or C++: use uint32_t
	i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
	i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
	return (i * 0x01010101) >> 24;          // horizontal sum of bytes
}

bool IsPEFile(const std::string& filePath)
{
    // Read the first few bytes of the file
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }

    std::vector<char> buffer(2); // Read the first two bytes
    file.read(buffer.data(), buffer.size());

    if (file.gcount() != buffer.size())
    {
        std::cerr << "Error reading from file: " << filePath << std::endl;
        return false;
    }

    // Check for the MZ signature
    if (buffer[0] == 'M' && buffer[1] == 'Z')
    {
        std::cout << "The file is a PE file." << std::endl;
        return true;
    }
    else
    {
        std::cout << "The file is not a PE file." << std::endl;
        return false;
    }
}

/*bool BeginCentered(const char* name)
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 pos(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings;
    return ImGui::Begin(name, nullptr, flags);
}*/