#pragma once
#include <RobloxManager.hpp>
#include <iostream>
#include <Log.hpp>
#include <Memory.hpp>
#include <Compressing.hpp>

RBX RobloxInstances::InitializeRobloxInstanceAsync(Process junk) {
	// DATAMODEL
	pointer _datamodel = Instance::GetDataModel(junk.SelfModule);
	Log::debug("Found Datamodel at: 0x%llx", _datamodel);

	Instance datamodel(_datamodel);
	Instance Workspace = datamodel.FindFirstChild("Workspace");
	Instance Camera = Workspace.FindFirstChild("Camera");
	Instance Players = datamodel.FindFirstChild("Players");
	Instance ScriptContext = datamodel.FindFirstChild("Script Context");
	Instance PlayerScripts = datamodel.FindFirstChild("StarterPlayer");



	// STARTERPLAYERSCRIPTS T->D VRNavigation
	pointer _starterplayerscripts = PlayerScripts.FindFirstChild("StarterPlayerScripts").GetPointer();
	Log::debug("Found StarterPlayerScripts at: 0x%llx", _starterplayerscripts);

	Instance StarterPlayerScripts(_starterplayerscripts);
	Instance PlayerModule = StarterPlayerScripts.FindFirstChild("PlayerModule");
	Instance ControlModule = PlayerModule.FindFirstChild("ControlModule");
	Instance VrNavigation = ControlModule.FindFirstChild("VRNavigation");


	// COREGUI T->D PLAYERLISTMANAGER
	pointer _coregui = datamodel.FindFirstChild("CoreGui").GetPointer();
	Log::debug("Found CoreGui at: 0x%llx", _coregui);

	Instance CoreGui(_coregui);
	Instance RobloxGui = CoreGui.FindFirstChild("RobloxGui");
	Instance ModulesFolder = RobloxGui.FindFirstChild("Modules");
	Instance PlayerList = ModulesFolder.FindFirstChild("PlayerList");
	Instance PlayerListManager = PlayerList.FindFirstChild("PlayerListManager");

	// LOCALPLAYER T->D HUMANOID, CHARACTER, NAME
	pointer _localplayer = Memory::read_mem<pointer>(Players.GetPointer() + Offsets::LocalPlayer, junk.ProcessHandle);
	Log::debug("Found LocalPlayer at: 0x%llx", _localplayer);

	Instance LocalPlayer(_localplayer);
	std::string name = LocalPlayer.GetName();
	Log::debug("Local Player Name: %s", name.c_str());
	Instance Character = Workspace.FindFirstChild(name);
	Instance Humanoid = Character.FindFirstChild("Humanoid");

	return RBX(
		{
			_datamodel,
			Workspace.GetPointer(),
			Camera.GetPointer(),
			Players.GetPointer(),
			ScriptContext.GetPointer(),
			PlayerScripts.GetPointer()
		},
		{
			_starterplayerscripts,
			PlayerModule.GetPointer(),
			ControlModule.GetPointer(),
			VrNavigation.GetPointer()
		},
		{
			_coregui,
			RobloxGui.GetPointer(),
			ModulesFolder.GetPointer(),
			PlayerList.GetPointer(),
			PlayerListManager.GetPointer()
		},
		{
			_localplayer,
			name,
			Character.GetPointer(),
			Humanoid.GetPointer()
		}
		);

}


int RobloxInstances::RequireBypass(uintptr_t ScriptContext, bool mode) {
	uint32_t val = static_cast<uint32_t>(mode); // omg wtf thats so pro

	Memory::write_mem<uint32_t>(ScriptContext + Offsets::RequireBypass, val, current.ProcessHandle);

	return val;
}

int RobloxInstances::CoreBypass(uintptr_t addr) {
	Memory::write_mem<uintptr_t>(addr + Offsets::ModuleFlags, 0x100000000, current.ProcessHandle);
	Memory::write_mem<uintptr_t>(addr + Offsets::IsCoreScript, 0x1, current.ProcessHandle); // just dont ask bruh
	return 1;
}


HMODULE GetResourceModule() {
	HMODULE Module;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetResourceModule, &Module);
	return Module;
}

bool RobloxInstances::Init(RBX rbx, HMODULE module, HRSRC Resource, HGLOBAL Data, void* LockData) {
	Memory::write_mem<uintptr_t>(rbx.coreGui.PlayerListManager + Offsets::Size, rbx.starterPlayerScripts.VrNavigation, current.ProcessHandle);
	RequireBypass(rbx.dataModel.ScriptContext, true); // cannot require a non-Roblox script....
	
	Instance _vrnavigation(rbx.starterPlayerScripts.VrNavigation);
	_vrnavigation.SetBytecode(LuauCompress::BytecodeXor(LuauCompress::Compile("getfenv(0)['script'] = nil coroutine['wrap'](function(...)" + std::string(static_cast<char*>(LockData), SizeofResource(module, Resource)) + "\nend)() while task['wait'](9e9) do task['wait'](9e9) end")));


	while (GetForegroundWindow() != current.WindowHandle) {
		SetForegroundWindow(current.WindowHandle);
		Sleep(1);
	}
	SendMessageW(current.WindowHandle, WM_CLOSE, NULL, NULL);

	Sleep(150);

	Memory::write_mem<uintptr_t>(rbx.coreGui.PlayerListManager + Offsets::Size, rbx.coreGui.PlayerListManager, current.ProcessHandle);

	Log::debug("Initial Run completed!");

	Instance _coregui(rbx.coreGui.Self);
	_coregui.WaitForChild("FinishedInit", 5);

	return true;
}