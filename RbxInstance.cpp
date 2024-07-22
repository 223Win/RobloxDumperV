#include "RBX.h"
#include "Injection.h" // Injection Lib 

namespace RBX {
    std::vector<RBX::Instance> Instance::GetChildren()
    {
        std::vector<RBX::Instance> ChildrenObject;

        if (!this->InstancePointerInt) {
            throw std::exception("No Instance Pointer! Invalid Instance");
        }
        else if (!this->InheritedRobloxHandle) {
            throw std::exception("No Inherited Roblox Handle! Invalid Instance Class");
        }

        RBX::Types::RbxInt Start = RBX::Memory::Read<RBX::Types::RbxInt>(this->InheritedRobloxHandle, this->InstancePointerInt + RBX::Offsets::Children);
        std::cout << "START OF CHILDREN: " << Start << std::endl;

        if (!Start) {
            return ChildrenObject;
        }

        RBX::Types::RbxInt End = RBX::Memory::Read<RBX::Types::RbxInt>(this->InheritedRobloxHandle, Start + RBX::Offsets::Size);
        std::cout << "END OF CHILDREN: " << End << std::endl;

        if (!End) {
            return ChildrenObject;
        }

        for (RBX::Types::RbxInt Inst = Start; Inst != End; Inst += RBX::Offsets::TableIterator)
        {
            ChildrenObject.emplace_back(Instance(Inst, this->InheritedRobloxHandle));
        }

        return ChildrenObject;
    }

    Instance Instance::Parent() {
        return Instance(this->InstancePointerInt + RBX::Offsets::Parent, this->InheritedRobloxHandle);
    }

    std::string Instance::Name() {
        return "NODATA";
    }

    Instance::Instance(RBX::Types::RbxInt instancePointerInt, HANDLE inheritedRobloxHandle)
        : InstancePointerInt(instancePointerInt), InheritedRobloxHandle(inheritedRobloxHandle) {
    }
}








