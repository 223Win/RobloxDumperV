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

static std::optional<std::string> ParseDir(const std::string& NameToGet, std::filesystem::directory_iterator Iteration) {
    for (const auto& entry : Iteration) {
        if (entry.is_directory()) {
            auto Result = ParseDir(NameToGet, std::filesystem::directory_iterator(entry.path()));
            if (Result.has_value()) {
                return Result;
            }
        }
        else if (entry.is_regular_file()) {
            //std::cout << entry.path().filename() << std::endl;
            if (entry.path().filename() == NameToGet) {
                return entry.path().string();
            }
        }
    }
    return std::nullopt;
}

static bool ChangeMemoryProtection(HANDLE processHandle, uintptr_t address, SIZE_T size, DWORD newProtect) {
    DWORD oldProtect;
    if (VirtualProtectEx(processHandle, (LPVOID)address, size, newProtect, &oldProtect)) {
        std::cout << "Memory protection changed successfully. Old protection: " << std::hex << oldProtect << std::endl;
        return true;
    }
    else {
        std::cerr << "Failed to change memory protection. Error: " << GetLastError() << std::endl;
        return false;
    }
}


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

    namespace MemorySecurity {
        // Low Level Memory Override
        extern "C" NTSTATUS NTAPI ZwProtectVirtualMemory(
            HANDLE ProcessHandle,
            PVOID* BaseAddress,
            PSIZE_T NumberOfBytesToProtect,
            ULONG NewProtect,
            PULONG OldProtect
        );
        
        // PH stands for Processhandle for you weirdos
        MEMORY_BASIC_INFORMATION QueryMemoryAddress(HANDLE PH, uintptr_t Address) {
            MEMORY_BASIC_INFORMATION mbi;
            ZeroMemory(&mbi, sizeof(mbi));

            // Query information about the memory page containing the address
            SIZE_T result = VirtualQueryEx(PH, (LPCVOID)Address, &mbi, sizeof(mbi));



            return mbi;
        }

        

        int GetMemorySecurity(HANDLE PH,uintptr_t Address) {
            return QueryMemoryAddress(PH, Address).Protect;
        }
        // Sets Memory's Protection
        // Returns the Previous Memory Protection
        BOOL SetMemorySecurity(HANDLE PH, uintptr_t Address) {
            MEMORY_BASIC_INFORMATION mbi = QueryMemoryAddress(PH, Address);

            if (mbi.RegionSize == 0 || mbi.BaseAddress == nullptr) {
                std::cerr << "Invalid base address or region size." << std::endl;
                return FALSE;
            }
            DWORD oldProtect = 0;
            std::cout << mbi.BaseAddress << std::endl;
            SIZE_T result =  VirtualProtectEx(PH, (LPVOID)Address, mbi.RegionSize, PAGE_READONLY, &oldProtect);
            if (result == 0) {
                std::cerr << "VirtualQueryEx failed. Error code: " << GetLastError() << std::endl;
                return result;
            }

            return result;

        }
    }

    namespace Utils {
        uintptr_t HexStringToDecimal(const std::string& str) {
            uintptr_t address = 0;
            std::stringstream ss;

            std::string formattedHexStr = str;
            if (formattedHexStr.find("0x") == 0) {
                formattedHexStr = formattedHexStr.substr(2);
            }

            ss << std::hex << formattedHexStr;
            ss >> address;
            std::cout << address << std::endl;
            return address;
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

        RBX::Types::RbxInt GetRenderView(std::string Line) {
            size_t LineLen = Line.length();
            size_t Offset = Line.find("initialize view(") + RBX_ZeroPointInternalOffset;
            size_t FixedOffset = Offset + RBX_DataModelInternalOffset;

            std::string unfixed = Line.substr(FixedOffset, LineLen);
            std::cout << unfixed << " ADDY" << std::endl;
            RBX::Types::RbxInt Hex = HexStringToDecimal(unfixed);
            std::cout << "Original Hex Address: " << unfixed << std::endl;
            std::cout << "Converted Address: " << std::hex << Hex << std::endl;
            return Hex;
        }
    }

    namespace Roblox {
        std::optional< std::string> GetRobloxPath()
        {
            std::string VersionsPath = RBX::Utils::GetAppData() + "\\Roblox\\Versions";
            auto Result = ParseDir("RobloxPlayerBeta.exe", std::filesystem::directory_iterator(VersionsPath));
            if (Result.has_value()) {
                std::cout << Result.value() << std::endl;
                return Result.value();
            }
            else {
                std::cout << "RobloxPlayerBeta not found." << std::endl;
                return std::nullopt;
            }

            return std::nullopt;
        }

        void CreateRobloxProcess() {
            STARTUPINFOA startupInfo = { 0 }; // Use STARTUPINFOA for ANSI version
            PROCESS_INFORMATION processInfo = { 0 };

            startupInfo.cb = sizeof(STARTUPINFOA);
            startupInfo.dwFlags |= STARTF_USESHOWWINDOW;
            startupInfo.wShowWindow = SW_SHOWNORMAL;

            BOOL success = CreateProcessA(
                GetRobloxPath().value().c_str(),      // Application name
                NULL,                         // Command line arguments
                NULL,                         // Process security attributes
                NULL,                         // Thread security attributes
                TRUE,                        // Inherit handles
                0,                            // Creation flags
                NULL,                         // Environment
                NULL,                         // Current directory
                &startupInfo,                 // STARTUPINFO structure
                &processInfo                  // PROCESS_INFORMATION structure
            );
            std::cout << success << std::endl;

            
        }
    }
}
