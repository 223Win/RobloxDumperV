#include "ClassDescriptor.h"
#include "TypeRegistery.h"

// Property stuff //

#include "Properties.h"
#include "TTypes.h"

// Type Register //

//#include "TypeRegistery.h"


#include "SmartDecoder.hpp"


namespace RBX {
	
	ClassDescriptor::ClassDescriptor(RBX::Instance Instance, HANDLE Handle) :
		InstanceObj(Instance),
		ClassDescriptorAddress(RBX::Memory::Read<uintptr_t>(Handle, Instance.InstancePointerInt + 0x18)),
		InheritedRobloxHandle(Handle)
	{}
	std::vector<PropertyCore> ClassDescriptor::GetProperties()
	{
		std::vector<PropertyCore> Children;

		uintptr_t Start = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, ClassDescriptorAddress + 0x28);
		uintptr_t End = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, ClassDescriptorAddress + 0x30);

		

		for (uintptr_t Child = Start; Child < End; Child += 8) {
			Children.emplace_back(PropertyCore(RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, Child), InheritedRobloxHandle));
		}

		return Children;
	}
	
	void ClassDescriptor::ParseProperties()
	{
		for (auto& Child : GetProperties()) {
			TTypeObject TType = Child.GetTType();
			//std::cout << Child.PropertyPointer << std::endl;
			std::string TypeName = TType.GetTypeName();
			bool IsSupported = TypeRegistry::IsTypeSupported(TypeName);
			if (TypeName == "bool") {
				bool Gotval = Child.GetPropertyValue<bool>();
				std::cout << "Prop Value: " << Gotval << std::endl;
			}
			//std::cout << Child.Name() << std::endl;
		}
	}
}
