#include "RBX.h"
#include <Windows.h>
#include "Injection.h"

namespace {
	RBX::Injection::NtOpenProcess_t NtOpenProcess;
	RBX::Injection::NtAllocateVirtualMemory_t NtAllocateVirtualMemory;
	RBX::Injection::NtWriteVirtualMemory_t NtWriteVirtualMemory;
	RBX::Injection::NtReadVirtualMemory_t NtReadVirtualMemory;
	RBX::Injection::NtCreateThreadEx_t NtCreateThreadEx;
	RBX::Injection::NtFreeVirtualMemory_t NtFreeVirtualMemory;
	RBX::Injection::NtClose_t NtClose;

	void InitOnce() {
		static bool initialized = false;
		if (!initialized) {
			RBX::Injection::InitFunctions(NtOpenProcess, NtAllocateVirtualMemory, NtWriteVirtualMemory, NtReadVirtualMemory, NtCreateThreadEx, NtFreeVirtualMemory, NtClose);
			initialized = true;
		}
	}

	struct Initializer {
		Initializer() {
			InitOnce();
		}
	} initializer;
}

namespace RBX {
	namespace Memory {
		template<typename T>
		T Read(void* ProcessHandle, unsigned long long Address) {
			T Buffer{};
			SIZE_T out = 0;
			NtReadVirtualMemory(ProcessHandle, (PVOID)Address, &Buffer, sizeof(T), &out);

			return Buffer;
		}

		template<typename T>
		void Write(void* ProcessHandle, unsigned long long Address, T NewValue) {
			SIZE_T out = 0;
			NtWriteVirtualMemory(ProcessHandle, (PVOID)Address, &NewValue, sizeof(T), &out);
		}
	}
}