#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include "RBX.h"

namespace RBX {
	
	class Instance {
	public:
		Instance(uintptr_t instancePointerInt, HANDLE inheritedRobloxHandle);
		std::vector<Instance> GetChildren();
		Instance Parent();
		std::string ClassName();
		std::string Name();
		void GetProperty(std::string Name);

		/*Instance(Instance&& other) noexcept = default;
		Instance& operator=(Instance&& other) noexcept = default;*/

		RBX::Types::RbxInt InstancePointerInt;
	private:

		/*Instance(const Instance&) = delete;
		Instance& operator=(const Instance&) = delete;*/

		uintptr_t ClassDescriptor;
		HANDLE InheritedRobloxHandle;
	};
}
