#include "Injection.h"
#include <string>

#define THREAD_CREATE 0x0002

void __stdcall ApcRoutine(PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, ULONG Reserved) {
    typedef void (*FuncType)();
    FuncType func = reinterpret_cast<FuncType>(ApcContext);
    func(); // Call the function
}

namespace RBX {
    namespace Injection {

        void InitFunctions(
            NtOpenProcess_t& NtOpenProcess, NtAllocateVirtualMemory_t& NtAllocateVirtualMemory, NtWriteVirtualMemory_t& NtWriteVirtualMemory, NtReadVirtualMemory_t& NtReadVirtualMemory, NtCreateThreadEx_t& NtCreateThreadEx, NtFreeVirtualMemory_t& NtFreeVirtualMemory, NtClose_t& NtClose)
        {
            HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
            if (!ntdll) {
                throw std::runtime_error("Failed to get handle of ntdll.dll");
            }

            NtOpenProcess = (RBX::Injection::NtOpenProcess_t)GetProcAddress(ntdll, "NtOpenProcess");
            NtAllocateVirtualMemory = (RBX::Injection::NtAllocateVirtualMemory_t)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
            NtWriteVirtualMemory = (RBX::Injection::NtWriteVirtualMemory_t)GetProcAddress(ntdll, "NtWriteVirtualMemory");
            NtReadVirtualMemory = (RBX::Injection::NtReadVirtualMemory_t)GetProcAddress(ntdll, "NtReadVirtualMemory");
            NtCreateThreadEx = (RBX::Injection::NtCreateThreadEx_t)GetProcAddress(ntdll, "NtCreateThreadEx");
            NtFreeVirtualMemory = (RBX::Injection::NtFreeVirtualMemory_t)GetProcAddress(ntdll, "NtFreeVirtualMemory");
            NtClose = (RBX::Injection::NtClose_t)GetProcAddress(ntdll, "NtClose");

            if (!NtOpenProcess || !NtAllocateVirtualMemory || !NtWriteVirtualMemory || !NtReadVirtualMemory ||
                !NtCreateThreadEx || !NtFreeVirtualMemory || !NtClose) {
                throw std::runtime_error("Failed to get address of one or more functions");
            }
        }

        HANDLE OpenProcessNt(HANDLE processId) {
            NtOpenProcess_t NtOpenProcess = (NtOpenProcess_t)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtOpenProcess");

            PCLIENT_ID clientId = { processId, NULL };
            OBJECT_ATTRIBUTES objAttr;
            InitializeObjectAttributes(&objAttr, NULL, 0, NULL, NULL);

            HANDLE hProcess = NULL;
            NTSTATUS status = NtOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, clientId);

            if (!NT_SUCCESS(status)) {
                return NULL;
            }

            return hProcess;
        }
        void CallFunctionInInjectedDLL(HANDLE processHandle, const char* functionName) {
            HMODULE dllModule = GetModuleHandleA(DLLNAME);
            if (!dllModule) {
                throw std::runtime_error("Failed to get handle of the injected DLL.");
            }

            FARPROC funcAddr = GetProcAddress(dllModule, functionName);
            if (!funcAddr) {
                throw std::runtime_error("Failed to get address of the function in the injected DLL.");
            }

            HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)funcAddr, NULL, 0, NULL);
            if (!remoteThread) {
                throw std::runtime_error("Failed to create remote thread.");
            }

            WaitForSingleObject(remoteThread, INFINITE);
            CloseHandle(remoteThread);
        }

        HANDLE InjectDLL(HANDLE processHandle, const char* dllPath, NtOpenProcess_t NtOpenProcess, NtAllocateVirtualMemory_t NtAllocateVirtualMemory, NtWriteVirtualMemory_t NtWriteVirtualMemory, NtCreateThreadEx_t NtCreateThreadEx, NtFreeVirtualMemory_t NtFreeVirtualMemory, NtClose_t NtClose) {
            PVOID remoteMemory = nullptr;
            SIZE_T pathSize = strlen(dllPath) + 1;
            NTSTATUS status = NtAllocateVirtualMemory(processHandle, &remoteMemory, 0, &pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (!NT_SUCCESS(status)) {
                throw std::runtime_error("NtAllocateVirtualMemory failed with status: " + std::to_string(status));
            }

            SIZE_T bytesWritten;
            status = NtWriteVirtualMemory(processHandle, remoteMemory, (PVOID)dllPath, pathSize, &bytesWritten);
            if (!NT_SUCCESS(status)) {
                NtFreeVirtualMemory(processHandle, &remoteMemory, &pathSize, MEM_RELEASE);
                throw std::runtime_error("NtWriteVirtualMemory failed with status: " + std::to_string(status));
            }

            HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
            FARPROC ldrLoadDllAddr = GetProcAddress(ntdll, "LdrLoadDll");
            if (!ldrLoadDllAddr) {
                NtFreeVirtualMemory(processHandle, &remoteMemory, &pathSize, MEM_RELEASE);
                throw std::runtime_error("GetProcAddress for LdrLoadDll failed.");
            }

            HANDLE remoteThread = nullptr;
            OBJECT_ATTRIBUTES objAttr;
            InitializeObjectAttributes(&objAttr, nullptr, 0, nullptr, nullptr);

            CLIENT_ID clientId;
            clientId.UniqueProcess = nullptr;
            clientId.UniqueThread = nullptr;

            status = NtCreateThreadEx(
                &remoteThread,
                THREAD_CREATE | THREAD_QUERY_INFORMATION,
                &objAttr,
                processHandle,
                (PVOID)ldrLoadDllAddr,
                remoteMemory,
                0,
                0,
                0,
                0,
                nullptr
            );
            if (!NT_SUCCESS(status)) {
                NtFreeVirtualMemory(processHandle, &remoteMemory, &pathSize, MEM_RELEASE);
                throw std::runtime_error("NtCreateThreadEx failed with status: " + std::to_string(status));
            }

            WaitForSingleObject(remoteThread, INFINITE);

            // Return remote thread handle
            return remoteThread;
        }
    }
}
