#pragma once
#include <iostream>

#include "RbxMem.hpp"



namespace RBX {
	namespace Getter {
		template<typename T>
		T GetValue(HANDLE Handle, uintptr_t Pointer) {
			return RBX::Memory::Read<T>(Handle, Pointer);
		}

		
		auto GetProperty(uintptr_t GetterAddress, auto BaseType);

		//--// Property Internals //--//
		template<typename T>
		using GetterFunc = T(__cdecl*)();
	}
}