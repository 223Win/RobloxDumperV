#include "HookManager.h"
#include <iostream>
#include <stdexcept>
#include <HookEngine/HookEngine.h>

HookManager::HookManager() = default;
HookManager::~HookManager() = default;

uintptr_t HookManager::GetFunctionAddress(const std::string& moduleName, const std::string& functionName) {
    HMODULE module = GetModuleHandleA(moduleName.c_str());
    if (!module) {
        throw std::runtime_error("Module not found.");
    }
    return reinterpret_cast<uintptr_t>(GetProcAddress(module, functionName.c_str()));
}

void HookManager::HookFunction(const std::string& moduleName, const std::string& functionName) {
    uintptr_t targetAddress = GetFunctionAddress(moduleName, functionName);
    if (targetAddress == 0) {
        throw std::runtime_error("Function not found.");
    }

    auto Hooker = HookEngine::createHook<
        HookEngine::Win64,
        int __cdecl,
        HWND,
        LPCSTR,
        LPCSTR,
        UINT,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty,
        HookEngine::_Empty
    >(GetFunctionAddress(moduleName, functionName));
    auto HookLamdba = [](auto arg1, auto arg2, auto arg3, auto arg4, auto arg5, auto arg6, auto arg7, auto arg8, auto arg9, auto arg10, auto arg11, auto arg12) -> int __cdecl {
        std::cout << "MBOX HOOK" << std::endl;
        std::cout << arg1 << std::endl;
        std::cout << arg2 << std::endl;
        std::cout << arg3 << std::endl;
        std::cout << arg4 << std::endl;
        return 0;
    };

    Hooker->assignHook(HookLamdba);
}

void HookManager::SetHookCallback(const std::string& hookName, FunctionPointer callback) {
    hooks[hookName] = callback;
}

void HookManager::TestHook() {
    try {
        // Example of hooking MessageBoxA function
        HookFunction("User32.dll", "MessageBoxA");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
