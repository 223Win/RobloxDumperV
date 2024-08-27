#pragma once
#include "RbxMem.hpp"
#include <vector>
#include <Windows.h>

namespace RBX {
	namespace Memory {
		template<typename T>
		T Read(HANDLE ProcessHandle, uintptr_t Address); // Forward Declare this (just errors if you remove it)
		template<typename T>
		void Write(HANDLE ProcessHandle, unsigned long long Address, T NewValue);
	}
}



namespace ExUtils {
	namespace ExternalFunctions {
		typedef std::tuple<uintptr_t, HANDLE> InternalProcessHookItem;
		typedef std::vector<InternalProcessHookItem> InternalProcessHookList; //int = Process Id, uintptr = Cached Function location;

		static ExUtils::ExternalFunctions::InternalProcessHookList IPHL; // Load up da list!
		
		void InitProcess(HANDLE ProcessHandle);
		void Cleanup();

		namespace {
			InternalProcessHookItem GetHookItemFromProcessHandle(HANDLE ProcessHandle) {
				for (auto Child : IPHL) {
					HANDLE val = std::get<1>(Child);
					if (val == ProcessHandle) {
						return Child;
					}
				}
			}
		}

		template<typename T, typename... Args>
		T CallFunction(HANDLE ProcessHandle, uintptr_t FunctionAddress) {
			/*auto Tuple = GetHookItemFromProcessHandle(ProcessHandle);
			uintptr_t RemoteMemory = std::get<0>(Tuple);*/

			// Shellcode to call the function at `functionAddress`
			unsigned char shellcode[] = {
				0x48, 0xB8,                   // mov rax, <address>
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <address> (64-bit)
				0xFF, 0xD0,                   // call rax
				0xC3                          // ret
			};

			// Replace placeholder address with actual function address
			*reinterpret_cast<uintptr_t*>(&shellcode[2]) = FunctionAddress;

			SIZE_T shellcodeSize = sizeof(shellcode); // Adjust size as needed
			LPVOID remoteMemory = VirtualAllocEx(ProcessHandle, NULL, shellcodeSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (remoteMemory == NULL) {
				std::cerr << "Failed to allocate memory in the target process!" << std::endl;
				return T{};
			}

			

			// Write the shellcode to the allocated memory
			SIZE_T bytesWritten;
			if (!WriteProcessMemory(ProcessHandle, remoteMemory, shellcode, sizeof(shellcode), &bytesWritten) || bytesWritten != sizeof(shellcode)) {
				std::cerr << "Failed to write shellcode to the target process!" << std::endl;
				VirtualFreeEx(ProcessHandle, remoteMemory, 0, MEM_RELEASE);
				return T{};
			}

			// Create a remote thread to execute the shellcode
			HANDLE remoteThread = CreateRemoteThread(ProcessHandle, NULL, 0, (LPTHREAD_START_ROUTINE)remoteMemory, NULL, 0, NULL);
			if (remoteThread == NULL) {
				std::cerr << "Failed to create remote thread!" << std::endl;
				VirtualFreeEx(ProcessHandle, remoteMemory, 0, MEM_RELEASE);
				return T{};
			}

			// Wait for the thread to complete
			WaitForSingleObject(remoteThread, INFINITE);

			DWORD exitcode = 0;
			GetExitCodeThread(remoteThread, &exitcode);
			std::cout << "Thread exitted with: " << exitcode << std::endl;

			return T{};
		}

	}
}