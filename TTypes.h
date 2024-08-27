#pragma once
#include "RBX.h"
#include "Properties.h"
#include <string>

namespace RBX {
    class PropertyCore;  // Forward declare PropertyCore
}

namespace RBX {
    class TTypeObject
    {
    public:
        uintptr_t TTypePointer;
        std::string GetTypeName() const;
        TTypeObject(PropertyCore* object, HANDLE RobloxHandle);

    private:



        HANDLE InheritedRobloxHandle;
        
    };
}
