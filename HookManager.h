#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>

class HookManager {
public:
    using FunctionPointer = void(*)(void);

    // Constructor and Destructor
    HookManager();
    ~HookManager();

    // Gets the address of a function in a specified module
    uintptr_t GetFunctionAddress(const std::string& moduleName, const std::string& functionName);

    // Sets up a hook for a given function
    void HookFunction(const std::string& moduleName, const std::string& functionName);

    // Registers a callback function for hooks
    void SetHookCallback(const std::string& hookName, FunctionPointer callback);

    // Test function to demonstrate hooking
    void TestHook();

private:
    // Stores hook functions by name
    std::unordered_map<std::string, FunctionPointer> hooks;
};
