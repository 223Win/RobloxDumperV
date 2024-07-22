#include "RBX.h"
#include "Injection.h" // Adds Injection API to RBX
#include "HookManager.h"
#include "RobloxDumperV.h"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <Windows.h>



int DoInject()
{
    RBX::Injection::NtOpenProcess_t NtOpenProcess;
    RBX::Injection::NtAllocateVirtualMemory_t NtAllocateVirtualMemory;
    RBX::Injection::NtWriteVirtualMemory_t NtWriteVirtualMemory;
    RBX::Injection::NtReadVirtualMemory_t NtReadVirtualMemory;
    RBX::Injection::NtCreateThreadEx_t NtCreateThreadEx;
    RBX::Injection::NtFreeVirtualMemory_t NtFreeVirtualMemory;
    RBX::Injection::NtClose_t NtClose;

    RBX::Injection::InitFunctions(NtOpenProcess, NtAllocateVirtualMemory, NtWriteVirtualMemory, NtReadVirtualMemory, NtCreateThreadEx, NtFreeVirtualMemory, NtClose);
    HANDLE processHandle = RBX::Injection::OpenProcessNt((HANDLE)RBX::Utils::GetRobloxProcessId());
    if (!processHandle) {
        throw std::runtime_error("Failed to open process.");
    }

    try {
        // Inject the DLL
        HANDLE remoteThread = RBX::Injection::InjectDLL(processHandle, DLLNAME, NtOpenProcess, NtAllocateVirtualMemory, NtWriteVirtualMemory, NtCreateThreadEx, NtFreeVirtualMemory, NtClose);

        // Call the exported function
        RBX::Injection::CallFunctionInInjectedDLL(processHandle, "ShowMessageBox");

        // Close the handle
        NtClose(processHandle);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        NtClose(processHandle);
        return -1;
    }

}

int main()
{
    //--// Secure Process //--//

    SetConsoleTitleA("RobloxPlayerBeta");


    //--// Check if Roblox is Active //--//

    int RobloxPid = RBX::Utils::GetRobloxProcessId();
    if (RobloxPid == -1) {
        std::cout << "Please Open Roblox!" << std::endl;
        return RBX::Errors::ROBLOX_NOT_OPEN;
    }

    //--// Init Core Functions //--//

    RBX::Injection::NtOpenProcess_t NtOpenProcess;
    RBX::Injection::NtAllocateVirtualMemory_t NtAllocateVirtualMemory;
    RBX::Injection::NtWriteVirtualMemory_t NtWriteVirtualMemory;
    RBX::Injection::NtReadVirtualMemory_t NtReadVirtualMemory;
    RBX::Injection::NtCreateThreadEx_t NtCreateThreadEx;
    RBX::Injection::NtFreeVirtualMemory_t NtFreeVirtualMemory;
    RBX::Injection::NtClose_t NtClose;

    RBX::Injection::InitFunctions(NtOpenProcess, NtAllocateVirtualMemory, NtWriteVirtualMemory, NtReadVirtualMemory, NtCreateThreadEx, NtFreeVirtualMemory, NtClose);

    //--// Core Program //--//


    bool InGame = true;
    std::cout << "Getting Current Roblox Log" << std::endl;
    std::filesystem::path CurrentLogFile = RBX::Utils::GetCurrentRbxLogFile();
    std::vector<std::string> Lines = RBX::Utils::ReverseLines(CurrentLogFile);
    std::cout << "Got File info; Log path: " << CurrentLogFile.string() << std::endl;
    for (size_t i = 0; i < Lines.size(); ++i) {
        if (Lines[i].find("[FLog::Network] NetworkClient:Remove") != std::string::npos) {
            if (InGame == true) {
                InGame = false;
            }
            else {
                std::cout << "Player is not in game, Valid Disconnect Detected. Aborting." << std::endl;
                return RBX::Errors::PLAYER_NOT_INGAME;
            }
        }
        if (Lines[i].find("[FLog::SingleSurfaceApp] doTeleport") != std::string::npos) {
            if (InGame == false) {
                InGame = true;
            }
        }
        if (Lines[i].find("initialized DataModel(") != std::string::npos) {
            if (InGame == false) {
                std::cout << "Player is not in game, Valid Disconnect Detected. Aborting." << std::endl;
                return RBX::Errors::PLAYER_NOT_INGAME;
            }
            std::string LineData = Lines[i];
            RBX::Types::RbxInt DataModelAddress = RBX::Utils::GetDataModel(LineData);
            std::cout << "Got DataModel Address" << std::endl;
            std::cout << "Address: " << DataModelAddress << std::endl;
            std::cout << "Roblox PID: " + std::to_string(RobloxPid) << std::endl;
            HANDLE RBXHANDLE = RBX::Injection::OpenProcessNt((HANDLE)RobloxPid);
            std::cout << RBX::Memory::Read<RBX::Types::RbxInt>(RBXHANDLE, DataModelAddress) << std::endl;
            RBX::Instance DataModel = RBX::Instance(DataModelAddress, RBXHANDLE);
            std::cout << "DATA MODEL ADDRESS: " << std::endl;
            std::cout << DataModelAddress << std::endl;
            std::cout << "ATTEMPTING TO GET DATAMODEL PARENT" << std::endl;
            std::cout << DataModel.Parent().InstancePointerInt << std::endl;
            std::cout << "ATTEMPTING TO GET CHILDREN" << std::endl;
            std::vector<RBX::Instance> Children = DataModel.GetChildren();
            std::cout << "Children Length: " << std::endl;
            std::cout << Children.size() << std::endl;
            return RBX::Errors::SUCCESS;
        }
        continue;
    }
}
