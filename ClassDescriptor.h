#pragma once
#include "RBX.h"

#include <iostream>
#include <vector>



namespace RBX {
	class PropertyCore;  // Forward declare PropertyCore
}
namespace RBX {
	class ClassDescriptor
	{
	public:
		ClassDescriptor(RBX::Instance instance, HANDLE handle);
		std::vector<PropertyCore> GetProperties();
		void ParseProperties();

		uintptr_t ClassDescriptorAddress;
	private:
		RBX::Instance InstanceObj;  // Ensure that Instance is fully defined
		HANDLE InheritedRobloxHandle;
	};
}
