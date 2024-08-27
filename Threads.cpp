#include "Threads.h"
#include "Windows.h"
#include <TlHelp32.h>

#include <iostream>

namespace ExUtils {
    namespace Threads {
        HANDLE GetFirstThread(int ProcessId)
        {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
            if (snapshot == INVALID_HANDLE_VALUE) return nullptr;

            THREADENTRY32 entry;
            entry.dwSize = sizeof(THREADENTRY32);

            if (Thread32First(snapshot, &entry)) {
                do {
                    if (entry.th32OwnerProcessID == ProcessId) {
                        HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
                        if (hThread) {
                            CloseHandle(snapshot);
                            return hThread;
                        }
                    }
                } while (Thread32Next(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return nullptr;
        }

        void CallFunctionFromThread(HANDLE Thread, uintptr_t FunctionAddress) {
            // Suspend the thread
            if (SuspendThread(Thread) == (DWORD)-1) {
                std::cerr << "Failed to suspend the thread." << std::endl;
                return;
            }

            // Initialize and get the thread context
            CONTEXT ThreadContext = {};
            ThreadContext.ContextFlags = CONTEXT_FULL;
            if (!GetThreadContext(Thread, &ThreadContext)) {
                std::cerr << "Failed to get the thread context." << std::endl;
                ResumeThread(Thread);
                return;
            }

            // Store the original context to restore it later
            CONTEXT OriginalContext = ThreadContext;

            // Modify the RIP to point to the function we want to call
            ThreadContext.Rip = (DWORD64)FunctionAddress;

            // Set the modified context back to the thread
            if (!SetThreadContext(Thread, &ThreadContext)) {
                std::cerr << "Failed to set the thread context." << std::endl;
                ResumeThread(Thread);
                return;
            }

            // Resume the thread to execute the function
            if (ResumeThread(Thread) == (DWORD)-1) {
                std::cerr << "Failed to resume the thread." << std::endl;
                return;
            }

            // Optionally, wait for the thread to execute the function.
            // Depending on your needs, you may need to introduce a delay or synchronization to ensure
            // the function executes before restoring the original context.
            Sleep(100); // Adjust this based on the function's execution time

            // Suspend the thread again to restore the original context
            if (SuspendThread(Thread) == (DWORD)-1) {
                std::cerr << "Failed to suspend the thread for context restoration." << std::endl;
                return;
            }

            // Restore the original context
            if (!SetThreadContext(Thread, &OriginalContext)) {
                std::cerr << "Failed to restore the original thread context." << std::endl;
                ResumeThread(Thread);
                return;
            }

            // Resume the thread to continue normal execution
            if (ResumeThread(Thread) == (DWORD)-1) {
                std::cerr << "Failed to resume the thread after restoring context." << std::endl;
            }

            std::cout << "Forced Thread to call function from RIP: " << FunctionAddress << std::endl;
        }
    }
}
