#include <iostream>
#include <Windows.h>
#include <thread>
#include <print>
#include <Overlay/Overlay.hpp>
#include <Overlay/Consolae.hpp>
#include <ProcessManager.hpp>
#include <Log.hpp>
#include <Memory.hpp>
#include <RobloxManager.hpp>
#include "Environment.h"
#include <Execution.hpp>

RBX instance;
auto i = &instance;

bool RenderImgui() {
    SetConsoleTitleA("IDEplLua");
    HWND window = ProcessHelper::ProcessManager::getHwnd("Roblox");
    if (!window) return false;
    auto RenderLoop = [&]() {
        while (true) {
            if (!ProcessHelper::ProcessManager::getHwnd("Roblox")) {
                Log::error("Roblox Closed or isnt accessable anymore!");
                std::cin.get();
                exit(EXIT_SUCCESS);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    std::thread(RenderLoop).detach();

    SetForegroundWindow(window);

    if (!OverlayHook->Initialize(instance)) {
        Log::error("Failed to initialize IDE overlay!");
        return EXIT_FAILURE;
    }
    return true;
}



HMODULE aa() { return nullptr;  }
int main()
{
    auto process = Memory::getprocess("Roblox");
    auto instance = RobloxInstances::InitializeRobloxInstanceAsync(process);
    *i = instance;

    std::thread([] {
        if (!RenderImgui()) {
            std::cin.get();
        }
    }).detach();
    
    Log::info("console -> Overlay.Console Initialized!");
    ConsoleLog::Info("Initialized!");
    
    ConsoleLog::Info("Welcome, %s", instance.localPlayer.name);
    Log::info("ScriptContext 0x%llx", instance.dataModel.ScriptContext);
    HMODULE module = nullptr;
    if (!GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCTSTR>(&aa),
        &module))
    {
        Log::error("Failed to get resource Module");
        std::cin.get();
        return false;
    }

    if (!module) {
        Log::error("Failed to get resource Module");
        std::cin.get();
        return false;
    }
    HRSRC Resource = FindResourceW(module, MAKEINTRESOURCEW(LuaI), MAKEINTRESOURCEW(Env));
    if (!Resource) {
        Log::error("Failed to get Init-Script");
        std::cin.get();
        return false;
    }
    HGLOBAL Data = LoadResource(module, Resource);
    if (!Data) {
        Log::error("Failed to Load Init-Script data!");
        std::cin.get();
        return false;
    }
    void* LockData = LockResource(Data);
    if (!LockData) {
        Log::error("Failed to Lock Resource!");
        std::cin.get();
        return false;
    }

    if (RobloxInstances::Init(instance, module, Resource, Data, LockData)) {
        ConsoleLog::Info("ATTACHED!");
        OverlayHook->NotifyImGui("Attached To Client!", "IDEplLua");
    }
    

    while (true) {
        ExecutionService::Execute("print('aaa')", instance);
        Sleep(1000);
    }

    exit(EXIT_SUCCESS);
}
