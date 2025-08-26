#pragma once
#include <Windows.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <Memory.hpp>
#include <Log.hpp>
#include <thread>


using pointer = uintptr_t;
using off = uint64_t; // uint64_t
using addr = uintptr_t; // uintptr_t 

namespace Offsets {
    inline addr FakeDatamodelPointer = 0x7022FA8;
    constexpr off DatamodelOffset = 0x1C0;


    constexpr off Name = 0x88;
    constexpr off Children = 0x68;
    constexpr off Parent = 0x58;
    constexpr off Size = 0x8;
    constexpr off Value = 0xD8;
    constexpr off RequireBypass = 0x7E8;
    constexpr off ClassDescriptor = 0x18;
    constexpr off IsCoreScript = 0x1A0;
    constexpr off ModuleFlags = IsCoreScript - 0x4;
    constexpr off ModuleScriptEmbedded = 0x158; // ModuleScriptBytecode -> CHAR
    constexpr off LocalPlayer = 0x128;

    // EXTERNAL
    constexpr off JumpPower = 0x1B8;
    constexpr off Walkspeed = 0x1DC;
    constexpr off WalkspeedCheck = 0x3B8;
    constexpr off Gravity = 0x998;
    constexpr off Health = 0x19C;
    constexpr off FOV = 0x168;
}


struct Process {
    HANDLE ProcessHandle;
    HWND WindowHandle;
    uintptr_t SelfModule;
};
inline Process current;

struct DataModel {
    pointer Self;
    pointer Workspace;       // Datamodel.FindFirstChild("Workspace")
    pointer Camera;          // Datamodel.FindFirstChild("Camera")
    pointer Players;         // Datamodel.FindFirstChild("Players")
    pointer ScriptContext;   // Datamodel.FindFirstChild("Script Context")
    pointer PlayerScripts;   // Datamodel.FindFirstChild("StarterPlayer") 
};

// StarterPlayerScripts -> PlayerModule -> ControlModule -> VRNavigation
struct StarterPlayerScripts {
    pointer Self;            // PlayerScripts.FindFirstChild("StarterPlayerScripts")
    pointer PlayerModule;    // StarterPlayerScripts.FindFirstChild("PlayerModule")
    pointer ControlModule;   // PlayerModule.FindFirstChild("ControlModule")
    pointer VrNavigation;    // ControlModule.FindFirstChild("VRNavigation")
};


// CoreGui -> RobloxGui -> Modules -> PlayerList -> PlayerListManager
struct CoreGui {
    pointer Self;              // Datamodel.FindFirstChild("CoreGui")
    pointer RobloxGui;         // CoreGui.FindFirstChild("RobloxGui")
    pointer ModulesFolder;     // RobloxGui.FindFirstChild("Modules")
    pointer PlayerList;        // ModulesFolder.FindFirstChild("PlayerList")
    pointer PlayerListManager; // PlayerList.FindFirstChild("PlayerListManager")
};

// Players -> LocalPlayer
struct LocalPlayer {
    pointer Self;           // Players + 0x128 == LocalPlayer
    std::string name;         // *LocalPlayer + 0x88 == const char* playername
    pointer Character;      // Workspace.FindFirstChild(Name)
    pointer Humanoid;       // Character.FindFirstChild("Humanoid")
};

// Main RBX struct (root)
struct RBX {
    DataModel dataModel;
    StarterPlayerScripts starterPlayerScripts;
    CoreGui coreGui;
    LocalPlayer localPlayer;
};


class Instance {
private:
    pointer instancePtr;
public:
    Instance(pointer ptr) : instancePtr(ptr) {}

    pointer GetPointer() const { return instancePtr; }

    static pointer GetDataModel(uintptr_t base) {
        pointer fake = Memory::read_mem<pointer>(base + Offsets::FakeDatamodelPointer, current.ProcessHandle);
        pointer real = Memory::read_mem<pointer>(fake + Offsets::DatamodelOffset, current.ProcessHandle);
        return real;
    }

    std::vector<Instance> GetChildren() const {
        std::vector<Instance> children;

        pointer childrensContainer = Memory::read_mem<pointer>(instancePtr + Offsets::Children, current.ProcessHandle);
        pointer childrens = Memory::read_mem<pointer>(childrensContainer, current.ProcessHandle);
        uintptr_t childrenSize = 0x700;

        for (uintptr_t i = 0; i < childrenSize; i += 0x10) {
            pointer childPtr = Memory::read_mem<pointer>(childrens + i, current.ProcessHandle);

            if (childPtr)
                children.emplace_back(childPtr);
        }

        return children;
    }

    Instance FindFirstChild(const std::string& name) const {
        constexpr int nameLength = 640;

        pointer childrensContainer = Memory::read_mem<pointer>(instancePtr + Offsets::Children, current.ProcessHandle);
        pointer top = Memory::read_mem<pointer>(childrensContainer, current.ProcessHandle);
        pointer end = Memory::read_mem<pointer>(childrensContainer + 0x8, current.ProcessHandle) + 1;


        for (uintptr_t childPtr = top; childPtr < end; childPtr += 0x10) {
            uintptr_t childAddr = Memory::read_mem<uintptr_t>(childPtr, current.ProcessHandle);
            if (childAddr == 0)
                break;

            uintptr_t namePtr = Memory::read_mem<uintptr_t>(childAddr + Offsets::Name, current.ProcessHandle);
            std::string nameBuffer = Memory::rbx_string(namePtr);

            if (nameBuffer == name) {
                return childAddr;
            }
        }

        return 0;
    }

    std::string ClassName() {
        uintptr_t ClassDescriptor = Memory::read_mem<uint64_t>(instancePtr + Offsets::ClassDescriptor, current.ProcessHandle);
        uintptr_t ClassNamePtr = Memory::read_mem<uint64_t>(ClassDescriptor + Offsets::Size, current.ProcessHandle);
        if (!ClassNamePtr)
            return "";
        uintptr_t Size = Memory::read_mem<size_t>(ClassNamePtr + 0x10, current.ProcessHandle);
        if (Size >= 16)
            ClassNamePtr = Memory::read_mem<uintptr_t>(ClassNamePtr, current.ProcessHandle);
        std::string Final;
        BYTE Code = 0;
        for (std::int32_t Index = 0; Code = Memory::read_mem<uint8_t>(ClassNamePtr + Index, current.ProcessHandle); Index++)
            Final.push_back(Code);

        return Final;

    }

    void SetBytecode(std::string Bytecode) {
        if (this->ClassName() != "ModuleScript") {
            Log::error("Attempt to set bytecode of a non-Modulescript!");
            return;
        }
        uintptr_t ModuleScriptBytecodePtr = Memory::read_mem<uintptr_t>(instancePtr + Offsets::ModuleScriptEmbedded, current.ProcessHandle);

        uintptr_t OldBytecode = Memory::read_mem<uintptr_t>(ModuleScriptBytecodePtr + 0x10, current.ProcessHandle);
        uintptr_t OldBytecodeSize = Memory::read_mem<uintptr_t>(ModuleScriptBytecodePtr + 0x20, current.ProcessHandle);

        auto restoreBytecode = [ModuleScriptBytecodePtr, OldBytecode, OldBytecodeSize]() {
            Sleep(1000);
            Memory::write_mem<uintptr_t>(ModuleScriptBytecodePtr + 0x10, OldBytecode, current.ProcessHandle);
            Memory::write_mem<uintptr_t>(ModuleScriptBytecodePtr + 0x20, OldBytecodeSize, current.ProcessHandle);
        };

        std::thread(restoreBytecode).detach();

        for (size_t i = 0; i < Bytecode.size(); i++) {
            Memory::write_mem<BYTE>(OldBytecode + i, static_cast<BYTE>(Bytecode[i]), current.ProcessHandle);
        }
        Memory::write_mem<uintptr_t>(ModuleScriptBytecodePtr + 0x20, Bytecode.size(), current.ProcessHandle);
    }


    uintptr_t ObjectValue() {
        return Memory::read_mem<uintptr_t>(instancePtr + Offsets::Value, current.ProcessHandle);
    }

    uintptr_t WaitForChild(const std::string& instanceName, int MaxTime = 500) {
        std::chrono::steady_clock::time_point Start = std::chrono::high_resolution_clock::now();
        auto Timeout = std::chrono::seconds(MaxTime);

        while (std::chrono::steady_clock::now() - Start <= Timeout) {
            uintptr_t Found = this->FindFirstChild(instanceName).GetPointer();
            if (Found)
                return Found;

            Sleep(100);
        }

        return 0;
    }

    Instance Parent() {
        return Memory::read_mem<uintptr_t>(instancePtr + Offsets::Parent, current.ProcessHandle);
    }

    std::string GetName() const {
        pointer namePtr = Memory::read_mem<pointer>(instancePtr + Offsets::Name, current.ProcessHandle);
        if (!namePtr) return "";

        std::string result;
        char c;
        do {
            c = Memory::read_mem<char>(namePtr + result.length(), current.ProcessHandle);
            if (c != '\0')
                result += c;
        } while (c != '\0' && result.length() < 64);

        return result;
    }

    bool IsValid() const { return instancePtr != 0; }
};





class RobloxInstances {
private:

public:
    static RBX InitializeRobloxInstanceAsync(Process proc);

    static int RequireBypass(uintptr_t ScriptContext, bool mode);
    static int CoreBypass(uintptr_t addr);



    static bool Init(RBX rbx, HMODULE module, HRSRC Resource, HGLOBAL Data, void* LockData);
};