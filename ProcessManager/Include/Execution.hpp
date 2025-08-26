#pragma once
#include <iostream>
#include "Compressing.hpp"
#include "RobloxManager.hpp"


namespace ExecutionService {
	inline void Execute(std::string Src, RBX rbx) {
		Instance _coregui(rbx.coreGui.Self);
		Instance ScriptContainer = _coregui.WaitForChild("CoreModule", 5);
		if (!ScriptContainer.GetPointer())
			return;

		Instance HolderValue = ScriptContainer.ObjectValue();
		if (!HolderValue.GetPointer())
			return;

		HolderValue.SetBytecode(LuauCompress::BytecodeXor(LuauCompress::Compile("return {['someExecution'] = function(...)\n" + Src + "\nend}")));
		RobloxInstances::CoreBypass(HolderValue.GetPointer());
	}
}
