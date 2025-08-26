#pragma once
#include <string>
#include <sol/sol.hpp>


/*
	THIS IS NOT LUAU!
	LUA 5.2!

*/

namespace Compiler {

	class Sandbox {
	private:
		static sol::state lua;

		static void Initialize();
	public:
		static void ExecuteScript(const std::string& code);
	};


}