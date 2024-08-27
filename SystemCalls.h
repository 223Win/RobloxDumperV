#pragma once
#include <Windows.h>
#include <string>


namespace System {
	template<typename T>
	T GetSystemFunction(std::string DllName, std::string FunctionName)
	{
		HMODULE DLL = GetModuleHandleA(DllName.c_str());
		
		if (!DLL) {
			throw std::runtime_error("Cannot Get Module Handle of: " + DllName + ", Are you sure that Module is Correct?");
		}

		T FuncCaller = (T)GetProcAddress(DLL, FunctionName.c_str());

		return FuncCaller; // No need to do error checking here cause Im lazy 
	}

}