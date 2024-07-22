#include "RBX.h"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <lmcons.h>
#include <shlobj.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <Windows.h>
#include <HookEngine/HookEngine.h>

namespace RBX {
    namespace Security {
        template<typename ReturnType, typename ...Args>
        FunctionPointer<ReturnType, Args...> GetFunctionByName(const std::string& name) {
            return FunctionInfo::GetFunction<ReturnType, Args...>(name);
        }
        uintptr_t GetFunctionAddress(const std::string& name) {
            auto func = GetFunctionByName<void, HWND, LPCSTR>(name);
            if (!func) return 0;
            return reinterpret_cast<uintptr_t>(func);
        }
        

        uintptr_t GetModuleFunctionAddress(const char* moduleName, const char* functionName) {
            HMODULE module = GetModuleHandleA(moduleName);
            if (!module) {
                return 0;
            }
            return reinterpret_cast<uintptr_t>(GetProcAddress(module, functionName));
        }

        void TestHook() {
            uintptr_t TargetSystemCall = GetModuleFunctionAddress("User32.dll", "MessageBoxA");
            uintptr_t TargetHookFunc = reinterpret_cast<uintptr_t>(GetFunctionByName<void, HWND, LPCSTR>("Hook1"));
            HookEngine::instance()->installHook(TargetSystemCall, TargetHookFunc);
        }
    }

    namespace Utils {
         RBX::Types::RbxInt HexStringToDecimal(const std::string& str) {
            return std::stoull(str, nullptr, 16);
        }

        template<typename ...Args>
        std::string string_format(const std::string& format, Args... args) {
            size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
            if (size <= 0) {
                throw std::runtime_error("Error during formatting.");
            }
            std::unique_ptr<char[]> buf(new char[size]);
            snprintf(buf.get(), size, format.c_str(), args...);
            return std::string(buf.get(), buf.get() + size - 1); // Exclude the '\0'
        }

        std::vector<std::string> ReverseLines(const std::filesystem::path& FilePath) {
            std::ifstream file(FilePath);
            std::vector<std::string> lines;
            std::string line;

            while (std::getline(file, line)) {
                lines.push_back(line);
            }

            std::reverse(lines.begin(), lines.end());
            return lines;
        }

        std::string GetAppData() {
            PWSTR path = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path))) {
                std::wstring ws(path);
                std::string localAppDataPath(ws.begin(), ws.end());
                CoTaskMemFree(path);
                return localAppDataPath;
            }
            else {
                throw std::runtime_error("Failed to get Local AppData folder path.");
            }
        }

        std::filesystem::path GetCurrentRbxLogFile() {
            std::filesystem::path mostRecentFile;
            std::filesystem::file_time_type mostRecentTime;

            for (const auto& entry : std::filesystem::directory_iterator(string_format("%s\\Roblox\\logs", GetAppData().c_str()))) {
                if (entry.is_regular_file()) {
                    std::string FileName = entry.path().filename().string();
                    if (FileName.find("_Studio_") != std::string::npos) {
                        continue;
                    }
                    auto currentFileTime = std::filesystem::last_write_time(entry);
                    if ((mostRecentFile.empty() || currentFileTime > mostRecentTime)) {
                        mostRecentFile = entry.path();
                        mostRecentTime = currentFileTime;
                    }
                }
            }

            return mostRecentFile;
        }

        int GetRobloxProcessId() {
            const char* processName = "RobloxPlayerBeta.exe";
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            if (snapshot == INVALID_HANDLE_VALUE) {
                throw std::runtime_error("Failed to create process snapshot.");
            }

            if (Process32First(snapshot, &entry)) {
                do {
                    std::wstring ws(entry.szExeFile);
                    std::string exeFile(ws.begin(), ws.end());
                    if (_stricmp(exeFile.c_str(), processName) == 0) {
                        CloseHandle(snapshot);
                        return static_cast<int>(entry.th32ProcessID);
                    }
                } while (Process32Next(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return -1;
        }

        RBX::Types::RbxInt GetDataModel(std::string Line) {
            size_t LineLen = Line.length();
            size_t Offset = Line.find("initialized DataModel(") + RBX_ZeroPointInternalOffset;
            size_t FixedOffset = Offset + RBX_DataModelInternalOffset;

            std::string unfixed = Line.substr(FixedOffset, LineLen);
            std::string Address = unfixed.substr(0, unfixed.length() - 1);
            RBX::Types::RbxInt Hex = HexStringToDecimal(Address);
            return Hex;
        }
    }
}
