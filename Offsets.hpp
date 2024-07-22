#pragma once
// Offsets.hpp (Roblox Internal Offsets)

namespace RBX {
    namespace Offsets {
        constexpr int Size = sizeof(RBX::Types::RbxInt); // Cant Verify If this is correct, thus checking offsets with ida
        constexpr int TableIterator = 0x10; // Cant Verify If this is correct, thus checking offsets with ida
        constexpr int Parent = 0x60;
        constexpr int Children = 0x50;
        constexpr int ClassDescriptor = 0x18;
        constexpr int Name = 0x48;
    }
}
