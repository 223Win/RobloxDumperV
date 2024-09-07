#include "RBX.h"
#include "Injection.h" // Adds Injection API to RBX
#include "ObjectInit.h"

#include "HookManager.h"

#include "RobloxDumperV.h"

#include "SystemCalls.h"
#include "ExternalFunctions.h"
#include "Threads.h"


#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>
#include <Windows.h>

#include "Security.h"

//--// To lazy to keep typing these out //--//

#define RBXINT RBX::Types::RbxInt
#define RbxInstance RBX::Instance
#define Table std::vector

DWORD WINAPI SimpleThreadFunction(LPVOID lpParam) {
    while (true) {
        Sleep(1000); // Sleep for 1 second
    }
    return 0;
}

bool IsProcessElevated() {
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD size;

    // Open the process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        std::cerr << "Failed to open process token. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Get the token elevation information
    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
        std::cerr << "Failed to get token information. Error: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);

    // Check if the process is elevated
    return elevation.TokenIsElevated != 0;
}

int main()
{
    Security::MainLoop();

    //--// Secure Process //--//
    
    SetConsoleTitleA("__unknown-title, UNAUTHORIZED ACCESS");
    //RBX::Roblox::GetRobloxPath();

    //--// Check if Roblox is Active //--//
        
    int RobloxPid = RBX::Utils::GetRobloxProcessId();
    
    if (RobloxPid == -1) {
        std::cout << "Please Open Roblox!" << std::endl;
        RBX::Roblox::CreateRobloxProcess();
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

    // Preload Internal Types //

    RBX::Objects::Init();


    bool IsAdministrator = IsProcessElevated();

    std::cout << "Is Administrator: " << IsAdministrator << std::endl;
    std::cout << "0: False, 1: True" << std::endl;
    
    bool InGame = true;
    std::cout << "Getting Current Roblox Log" << std::endl;
    std::filesystem::path CurrentLogFile = RBX::Utils::GetCurrentRbxLogFile();
    Table<std::string> Lines = RBX::Utils::ReverseLines(CurrentLogFile);
    std::cout << "Got File info; Log path: " << CurrentLogFile.string() << std::endl;
    for (size_t i = 0; i < Lines.size(); ++i) {
        if (Lines[i].find("[FLog::Network] NetworkClient:Remove") != std::string::npos) {
            if (InGame == true) {
                InGame = false;
            }
            else {
               // std::cout << "Player is not in game, Valid Disconnect Detected. Aborting." << std::endl;
               // return RBX::Errors::PLAYER_NOT_INGAME;
            }
        }
        if (Lines[i].find("[FLog::SingleSurfaceApp] doTeleport") != std::string::npos) {
            if (InGame == false) {
                InGame = true;
            }
        }
        if (Lines[i].find("initialize view(") != std::string::npos) {
            if (InGame == false) {
                std::cout << "Cannot Fetch Player Status (INJECTION)" << std::endl;
                //return RBX::Errors::PLAYER_NOT_INGAME;
            }
          
            std::string LineData = Lines[i];
            RBXINT RenderView = RBX::Utils::GetRenderView(LineData);
            auto Access = (IsAdministrator == false) ? ( PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_CREATE_THREAD) : PROCESS_ALL_ACCESS;
            //RBX::Injection::OpenProcessNt((HANDLE)RobloxPid)
            std::cout << "ACCESS TO PROCESS: " << Access << std::endl;
            HANDLE RBXHANDLE = OpenProcess(Access,false, RobloxPid);
            ExUtils::ExternalFunctions::InitProcess(RBXHANDLE); // Init Process
            //ExUtils::Threads::CallFunctionFromThread(RBXHANDLE, ExUtils::Threads::GetFirstThread(RobloxPid));
            //uintptr_t RealAddress = RBX::Memory::Read<uintptr_t>(RBXHANDLE, ClientReplicatorAddress);
            //std::cout << RealAddress << std::endl;
            std::cout << "HANDLE ID: " << RBXHANDLE << std::endl;
            //Instance ClientReplicator = Instance(ClientReplicatorAddress, RBXHANDLE);
            uintptr_t DataModelPtr = RBX::Memory::Read<uintptr_t>(RBXHANDLE, RenderView + 0x118);
            RbxInstance* DataModel = new RBX::Instance(RBX::Memory::Read<uintptr_t>(RBXHANDLE, DataModelPtr + 0x198), RBXHANDLE);
            DataModelPtr = NULL;
            
            //ExUtils::Threads::CallFunctionFromThread(ThreadMain, 0x7ff6d3f56ed0);
            
            std::cout << "Render View Address: " << RenderView << std::endl;
            
            std::cout << "Address: " << DataModel->InstancePointerInt << std::endl;
            //std::cout << "ClassDescr- Address: " << RBX::Memory::Read<uintptr_t>(RBXHANDLE, DataModel->InstancePointerInt + 0x18) << std::endl;
            std::cout << "Name: " << DataModel->Name() << std::endl;
            std::cout << "ClassName: " << DataModel->ClassName() << std::endl;

            std::vector<RbxInstance> Children = DataModel->GetChildren();
            
            for (auto Child: Children) {
                std::cout << Child.Name() << std::endl;
                std::cout << "Address: " << Child.InstancePointerInt << std::endl;
                //std::cout << Child.InstancePointerInt << std::endl;
                
                //Child.GetProperty("random");
                //std::cout << Child.ClassName() << std::endl;

            }
            //std::cout <<  << std::endl;

           /* std::cout << ClientReplicator.Parent() << std::endl;
            std::cout << ReadMemory<int>(RBXHANDLE, ClientReplicatorAddress + 0x48) << std::endl;
            std::cout << ReadMemory<std::string>(RBXHANDLE, RBX::Memory::Read<int>(RBXHANDLE, ClientReplicatorAddress + 0x60)+RBX::Offsets::Name) << std::endl;*/

            
            //--// Deprecated (We got a new method!) //--//
            /*std::string LineData = Lines[i];
            RBX::Types::RbxInt DataModelAddress = RBX::Utils::GetDataModel(LineData);
            std::cout << "Got DataModel Address" << std::endl;
            std::cout << "Address: " << DataModelAddress << std::endl;
            std::cout << "Roblox PID: " + std::to_string(RobloxPid) << std::endl;
            HANDLE RBXHANDLE = RBX::Injection::OpenProcessNt((HANDLE)RobloxPid);
            std::cout << RBX::Memory::Read<RBX::Types::RbxInt>(RBXHANDLE, DataModelAddress) << std::endl;
            RBX::Instance DataModelEntry = RBX::Instance(DataModelAddress, RBXHANDLE);
            RBX::Instance DataModel = DataModelEntry.Parent();
            RBX::Types::RbxStr TestName = RBX::Memory::Read<RBX::Types::RbxStr>(RBXHANDLE, RBX::Memory::Read<RBX::Types::RbxInt>(RBXHANDLE, DataModel.InstancePointerInt + 0x48)+0x8);
            std::cout << &TestName << std::endl;
            std::vector<RBX::Instance> Child = DataModelEntry.GetChildren();

            std::cout << Child.size() << std::endl;*/
            ExUtils::ExternalFunctions::Cleanup(); //--// Cleans up stack //--//
            NtClose(RBXHANDLE); //--// Prevents Mem Corruption On our side
            return RBX::Errors::SUCCESS;
        }
        continue;
    }
}
