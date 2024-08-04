#pragma once

#include <Windows.h>
#include <iostream>
#include "WindowsTypeChecker.h"

// TODO: Template specialise to avoid issues when passing in the wrong template.
// See example below
/*
template<typename T>
class my_types {};

template<typename K, typename V, template<typename> typename C = my_array>
class Map
{
    C<K> key;
    C<V> value;
};

#include <type_traits>

template<
    typename T, //real type
    typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
> struct S{};

int main() {
   S<int> s; //compiles
   S<char*> s; //doesn't compile
}

CONSTEXPR BOOLEAN IsTemplateVoid = std::is_same_v<ReturnType, VOID>;
if constexpr (std::is_same_v(ReturnType, DWORD)) {
    // good
}
*/


namespace PE
{
    class PEBase
    {
    public:
        virtual ~PEBase() {}

        // Read data from a specific section
        template<typename T = DWORD, typename In>
        T ReadFromSection(In offset)
        {
            CheckIsWindowsTypes<T>(); // If this fails, we simply don't compile
            if (offset + sizeof(T) <= m_imageSize) {
                return *reinterpret_cast<T*>(reinterpret_cast<DWORD>(GetBaseAddress()) + offset);
            }
            else {
                std::cerr << "Attempted to read outside the PE file boundary." << std::endl;
                return T();  // Default value if out of bounds
            }
        }

        //Callback
        //template<typename T = DWORD, typename In>
        //void WriteToSection(T data, In offset);

        // Get the size of the image
        DWORD GetImageSize() const { return m_imageSize; }

    protected:
        virtual LPVOID GetBaseAddress() const = 0;
        DWORD m_imageSize = NULL;
    };

};