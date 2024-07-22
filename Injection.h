#pragma once
#include <windows.h>
#include <iostream>
#include <stdexcept>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

#ifndef _PCLIENT_ID_DEFINED
#define _PCLIENT_ID_DEFINED

typedef struct _PCLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} PCLIENT_ID;

#endif

#define DLLNAME "./RbxInjector.dll"

namespace RBX {
    namespace Injection {

        // NT API
        typedef NTSTATUS(NTAPI* NtQueueApcThread_t)(HANDLE ThreadHandle,PIO_APC_ROUTINE ApcRoutine,PVOID ApcArgument1,PVOID ApcArgument2,PVOID ApcArgument3);
        typedef NTSTATUS(NTAPI* NtOpenProcess_t)(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId);
        typedef NTSTATUS(NTAPI* NtAllocateVirtualMemory_t)(HANDLE ProcessHandle, PVOID* BaseAddress, ULONG ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
        typedef NTSTATUS(NTAPI* NtWriteVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T NumberOfBytes, PSIZE_T NumberOfBytesWritten);
        typedef NTSTATUS(NTAPI* NtReadVirtualMemory_t)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);
        typedef NTSTATUS(NTAPI* NtCreateThreadEx_t)(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle, PVOID StartAddress, PVOID Argument, ULONG CreateFlags, ULONG ZeroBits, ULONG StackSize, ULONG MaximumStackSize, PVOID AttributeList);
        typedef NTSTATUS(NTAPI* NtFreeVirtualMemory_t)(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG FreeType);
        typedef NTSTATUS(NTAPI* NtClose_t)(HANDLE Handle);

        // Definitions //
        void InitFunctions(NtOpenProcess_t& NtOpenProcess, NtAllocateVirtualMemory_t& NtAllocateVirtualMemory, NtWriteVirtualMemory_t& NtWriteVirtualMemory, NtReadVirtualMemory_t& NtReadVirtualMemory, NtCreateThreadEx_t& NtCreateThreadEx, NtFreeVirtualMemory_t& NtFreeVirtualMemory, NtClose_t& NtClose);

        // Functions //
        void CallFunctionInInjectedDLL(HANDLE, const char*);
        HANDLE InjectDLL(HANDLE processHandle, const char* dllPath, NtOpenProcess_t NtOpenProcess, NtAllocateVirtualMemory_t NtAllocateVirtualMemory, NtWriteVirtualMemory_t NtWriteVirtualMemory, NtCreateThreadEx_t NtCreateThreadEx, NtFreeVirtualMemory_t NtFreeVirtualMemory, NtClose_t NtClose);
        HANDLE OpenProcessNt(HANDLE processId);
    }
}
