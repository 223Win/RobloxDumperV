#pragma once
#include <Windows.h>

namespace ExUtils {
	namespace Threads {
		HANDLE GetFirstThread(int);
		void CallFunctionFromThread(HANDLE, uintptr_t);
	}
}