#include "ExternalFunctions.h"

#include "RBX.h"

#include <vector>
#include <tuple>

static ExUtils::ExternalFunctions::InternalProcessHookList IPHL; // Load up da list!

namespace ExUtils {
	namespace ExternalFunctions {
		void InitProcess(HANDLE ProcessHandle)
		{
			uintptr_t RemoteMem = (uintptr_t)VirtualAllocEx(ProcessHandle, NULL, 2000, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

			std::tuple<uintptr_t, HANDLE> Data(RemoteMem, ProcessHandle);

			IPHL.emplace_back(Data);
		}
		void Cleanup()
		{
			for (auto Object : IPHL) {
				VirtualFreeEx(std::get<1>(Object), (LPVOID)std::get<0>(Object), 0, MEM_RELEASE);
			}
		}
		
	}
}