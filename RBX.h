#pragma once
#include "RbxTypes.h"
#include "RbxInstance.h"
#include "RbxMem.hpp"
#include "Offsets.hpp"
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <Windows.h>

namespace RBX {
    #define RBX_DataModelInternalOffset 23
    #define RBX_ZeroPointInternalOffset 1
    #define RBX_ALLACCESS 1

    template<typename ReturnType, typename ...Args>
    using FunctionPointer = ReturnType(*)(Args...);
    namespace Types {
        typedef unsigned long long RbxInt;
    }
    namespace Errors {
        enum ErrorCodes
        {
            SUCCESS = 0,
            ROBLOX_NOT_OPEN = -3000,
            DATAMODEL_NOT_FOUND = -3001,
            PLAYER_NOT_INGAME = -3002,
            INVALID_ADDRESS = -3003,
            HOOK_INITFAIL = -3004,
            HOOK_FAIL = -3005,
            ENABLE_HOOK_FAIL = -3006,
            ROBLOX_NO_HANDLE = -3007,
            NO_INSTANCE_POINTER = -3008,
            UNKNOWN_ERROR = -2999,
        };
    }

    

    namespace Security {
        uintptr_t GetFunctionAddress(const std::string& name);
        uintptr_t GetModuleFunctionAddress(const char*, const char*);
        void TestHook();
    }

    namespace FunctionInfo {
        template<typename ReturnType, typename ...Args>
        static std::unordered_map<std::string, FunctionPointer<ReturnType, Args...>> functionRegistry;

        template<typename ReturnType, typename ...Args>
        void RegisterFunction(const std::string& name, FunctionPointer<ReturnType, Args...> func) {
            functionRegistry<ReturnType, Args...>[name] = func;
        }

        template<typename ReturnType, typename ...Args>
        FunctionPointer<ReturnType, Args...> GetFunction(const std::string& name) {
            auto it = functionRegistry<ReturnType, Args...>.find(name);
            if (it != functionRegistry<ReturnType, Args...>.end()) {
                return it->second;
            }
            return nullptr;
        }
    }

    namespace Utils {
        RBX::Types::RbxInt HexStringToDecimal(const std::string&);
        RBX::Types::RbxInt  GetDataModel(std::string);
        int GetRobloxProcessId();
        template<typename ...Args>
        std::string string_format(const std::string&, Args... args);
        std::string GetAppData();
        std::filesystem::path GetCurrentRbxLogFile();
        std::vector<std::string> ReverseLines(const std::filesystem::path&);
    }
}
