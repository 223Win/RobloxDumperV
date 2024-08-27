#pragma once
#include "RBX.h"
#include "Injection.h"

#include "SystemCalls.h"

#include <Windows.h>

namespace {
	static RBX::Injection::NtReadVirtualMemory_t ReadM = System::GetSystemFunction<RBX::Injection::NtReadVirtualMemory_t>("ntdll.dll", "NtReadVirtualMemory");
	static RBX::Injection::NtWriteVirtualMemory_t WriteM = System::GetSystemFunction<RBX::Injection::NtWriteVirtualMemory_t>("ntdll.dll", "NtWriteVirtualMemory");
}



namespace RBX {
	namespace Memory {
		template<typename T>
		T Read(HANDLE ProcessHandle, unsigned long long Address) {
			T buffer;
			PULONG bytesRead = 0;
			NTSTATUS Status = ReadM(ProcessHandle, reinterpret_cast<PVOID>(Address), &buffer, sizeof(T), bytesRead);
			if (!NT_SUCCESS(Status)) {
				/*std::cout << "Failed to read memory at address: " << std::hex << Address << std::endl;
				std::cout << "Fail Code: " << Status << std::endl;
				std::cout << "Returning Base Value" << std::endl;*/
				return T{};
			}
			return buffer;
		}

		template<typename T>
		void Write(HANDLE ProcessHandle, unsigned long long Address, T NewValue) {
			PSIZE_T out = 0;
			WriteM(ProcessHandle, (PVOID)Address, &NewValue, sizeof(T), out);
		}
	}
}