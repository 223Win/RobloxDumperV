#pragma once
#include "RbxMem.hpp"
#include "TTypes.h"
#include "ClassDescriptor.h"

#include "ExternalFunctions.h"
#include "Threads.h"

//--// Property Internals //--//
template<typename T>
using GetterFunc = T(__cdecl*)();
template<typename T>
using SetterFunc = void(__cdecl*)(T);

namespace RBX {
	namespace Memory {
		template<typename T>
		T Read(HANDLE ProcessHandle, uintptr_t Address); // Forward Declare this (just errors if you remove it)
	}
}

namespace ExUtils {
	namespace ExternalFunctions {
		template<typename T>
		T CallFunction(HANDLE ProcessHandle, uintptr_t FunctionAddress, T Args);
	}
}



namespace RBX {
	class PropertyCore {
	public:
		PropertyCore(uintptr_t PropPointer, HANDLE RobloxHandle);
		TTypeObject GetTType() const;
		uintptr_t PropertyPointer;

		std::string Name();
		std::string Category();

		template<typename T>
		T GetPropertyValue() const {
			uintptr_t PropertyGetterSetterBaseClass = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, PropertyPointer + 0x80);
			uintptr_t PropertyGetterSetterVTable = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, PropertyGetterSetterBaseClass + 0x0);
			std::cout << "Got VTABLE: " << PropertyGetterSetterVTable << std::endl;
			uintptr_t PropertyGetterFunction = RBX::Memory::Read<uintptr_t>(InheritedRobloxHandle, PropertyGetterSetterVTable + 0x8);

			//ExUtils::Threads::CallFunctionFromThread(ExUtils::Threads::GetFirstThread(GetProcessId(InheritedRobloxHandle)), PropertyGetterFunction);
			T Result = ExUtils::ExternalFunctions::CallFunction<T>(InheritedRobloxHandle, PropertyGetterFunction);
			
			return Result;
		}

	private:
		HANDLE InheritedRobloxHandle;
		TTypeObject* TTypeObjectRef; 
	};
}
