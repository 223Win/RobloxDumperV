#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include "RBX.h"

namespace RBX {
	class Instance {
	public:
		Instance(RBX::Types::RbxInt instancePointerInt, HANDLE inheritedRobloxHandle);
		std::vector<Instance> GetChildren();
		Instance Parent();
		std::string Name();


		RBX::Types::RbxInt InstancePointerInt;
	private: 
		
		HANDLE InheritedRobloxHandle;
	};
}
