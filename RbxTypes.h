#pragma once

#include <array>
#include <string>


namespace RBX {
    namespace Types {
        typedef unsigned long long RbxInt;
        using RbxStr = std::basic_string<char, std::char_traits<char>, std::allocator<char>>;

        typedef std::array<char, 200> InstanceName;
        typedef std::array<char, 1000> ClassName; // Needs to be 1000 or less or strings wont decode 
    }
}