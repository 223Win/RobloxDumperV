#include "RBX.h"
#include "Injection.h" // Injection Lib 
#include "SmartDecoder.hpp" // Smart Decoder
#include "ClassDescriptor.h" // Class Descriptors

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

        uintptr_t StartPtr = RBX::Memory::Read<uintptr_t>(this->InheritedRobloxHandle, this->InstancePointerInt + RBX::Offsets::Children);
        uintptr_t Start = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, StartPtr);

        uintptr_t End = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, StartPtr + RBX::Offsets::Size);


        for (uintptr_t Inst = Start; Inst != End; Inst += 16)
        {
            std::cout << RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, Inst) << std::endl;
            ChildrenObject.emplace_back(Instance(RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, Inst), this->InheritedRobloxHandle));
        }
        Start = NULL;
        StartPtr = NULL;
        End = NULL;
        return ChildrenObject;
    }
    void Instance::GetProperty(std::string Name) {
        RBX::ClassDescriptor(*this, InheritedRobloxHandle).ParseProperties();
    }
    Instance Instance::Parent() {
        return Instance(RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, InstancePointerInt + RBX::Offsets::Parent), InheritedRobloxHandle);
    }
    std::string Instance::ClassName() {
        uintptr_t ClassNamePointer = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, ClassDescriptor + 0x8);
       
        //std::cout << std::string(ClassName.data()) << " || " << std::string(DecodedName.data()) << std::endl;
        //std::cout << std::string(DecodedName.data()) << std::endl;
        /*if (IndefPtrEncoded) {
            std::cout << RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, ClassNamePointer) << " Smart Decode Tester" << std::endl;
        }*/
        return RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, ClassNamePointer);
    }
    std::string Instance::Name() {
        uintptr_t NamePointer = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, InstancePointerInt + RBX::Offsets::Name);
        
        return RBX::SmartDecoder::DecodeString(InheritedRobloxHandle, NamePointer); // Just going to use Smart Decoder from now on cause its just easier
    }

    Instance::Instance(uintptr_t instancePointerInt, HANDLE inheritedRobloxHandle)
        : InstancePointerInt(instancePointerInt), 
        InheritedRobloxHandle(inheritedRobloxHandle), 
        ClassDescriptor(RBX::Memory::Read<uintptr_t>(inheritedRobloxHandle,instancePointerInt+0x18)) 
    {

    }
}








