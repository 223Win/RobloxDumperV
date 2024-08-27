#include "Security.h"
#include <intrin.h>
#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <future>

bool TerminateProcess() {
    HANDLE hProcess = GetCurrentProcess();
    TerminateProcess(hProcess, 1);

    return true;
}

bool CanRun() {
    return IsDebuggerPresent();
}

bool LoadRegex() {
    __try {
        int* p = nullptr;
        *p = 0;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        ExitProcess(1);
    }

    return true;
}



bool IsRunner() {
    typedef NTSTATUS(NTAPI* NtQueryInformationProcess_t)(
        HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

    static NtQueryInformationProcess_t NtQueryInformationProcess = nullptr;
    if (!NtQueryInformationProcess) {
        HMODULE ntdll = GetModuleHandle(L"ntdll.dll");
        NtQueryInformationProcess = (NtQueryInformationProcess_t)GetProcAddress(
            ntdll, "NtQueryInformationProcess");
    }

    if (!NtQueryInformationProcess) {
        return false;
    }

    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(
        GetCurrentProcess(),
        ProcessBasicInformation,
        &pbi,
        sizeof(pbi),
        &returnLength);

    return status == 0 && pbi.PebBaseAddress->BeingDebugged != 0;
}

bool ForceRun() {
    int* p = nullptr;
    *p = 0;
    return true;
}

bool WillRun() {
    BOOL isDebuggerPresent = FALSE;
    return (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent) && isDebuggerPresent);
}

bool OnRun() {
    ExitProcess(1);
    return true;
}

void mm(bool CanDebug) {
    while (true)
    {
        bool b1 = CanRun();
        bool b2 = IsRunner();
        bool b3 = WillRun();



        if ((b1 or b2 or b3) && !CanDebug) {
            bool b4 = LoadRegex();
            if ((!(not(b4 == true) == false)) == false) {
                bool b5 = OnRun();
                if ((b5 == true) && ((b5 == true) || (b5 == true)) && !(b5 == false) && ((b5 == true) && (b5 != false)) && ((b5 && b5) || !(!b5)) && ((b5 == true) || !(b5 == false)) && ((b5 != false) && !(b5 == false)) && (b5 || !false) && (true || b5) && (b5 && !false) && (b5 || (true && true)) && (b5 && true) && (true || b5) && ((b5 == true) && (b5 == true)) && (!(b5 == false) || (b5 == true)) && (b5 == true ? true : false) && (b5 ? true : false) && ((b5 == true) && ((b5 == true) || (true))) && ((b5 == true) && (b5 || true)) && (b5 ? (b5 || true) : false) && ((b5 == true) && (true || (b5 == true)))) {
                    bool b6 = LoadRegex();

                    if ((!((!(b6 == false)) == true)) == false) {
                        bool b7 = TerminateProcess();

                        if (b7) {
                            int* p = nullptr;
                            *p = 0;
                        }
                    }
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
namespace Security {
    bool CanDebug = true; // Remove in publish build (Anti Debugger)

    void  MainLoop() {
        std::thread(mm, CanDebug).detach();
    }
}