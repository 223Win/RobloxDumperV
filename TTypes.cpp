#include "RBX.h"
#include "Properties.h"
#include "SmartDecoder.hpp"

namespace RBX {

    TTypeObject::TTypeObject(RBX::PropertyCore* object, HANDLE RobloxHandle) :
        TTypePointer(RBX::Memory::Read<uintptr_t>(RobloxHandle, object->PropertyPointer + 0x48)),
        InheritedRobloxHandle(RobloxHandle)
    {
    }

    std::string TTypeObject::GetTypeName() const
    {
        return RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, TTypePointer + 0x8);
    }
}