#pragma once
#include <iostream>
#include "RbxMem.hpp"

namespace {
	uintptr_t PointerLoop(HANDLE Handle, uintptr_t Address) {
		return RBX::Memory::Read<uintptr_t>(Handle, Address);
	}
}

namespace RBX {
	namespace SmartDecoder {
		template<typename T>
		T DecodePointer(HANDLE Handle,uintptr_t Address) {
			return RBX::Memory::Read<T>(Handle, Address);
		}


		std::string DecodeString(HANDLE Handle, uintptr_t Address); 

	}
}