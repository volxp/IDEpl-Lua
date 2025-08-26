#include "Sandbox.hpp"
#include <Overlay/Consolae.hpp>

sol::state Compiler::Sandbox::lua;


void Compiler::Sandbox::Initialize() {
    static bool Initialized = false;
    if (Initialized) return;

    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);

    lua.set_function("print", [](sol::variadic_args args) {
        std::string output;
        for (auto v : args) {
            output += v.as<std::string>() + " ";
        }
        ConsoleLog::Info("%s", output.c_str());
        });

    lua.set_function("warn", [](sol::variadic_args args) {
        std::string output;
        for (auto v : args) {
            output += v.as<std::string>() + " ";
        }
        ConsoleLog::Warn("%s", output.c_str());
        });

    lua.set_function("error", [](sol::variadic_args args) {
        std::string output;
        for (auto v : args) {
            output += v.as<std::string>() + " ";
        }
        ConsoleLog::Error("%s", output.c_str());
        });

    Initialized = true;
}


void Compiler::Sandbox::ExecuteScript(const std::string& code) {
    Initialize();

    sol::protected_function_result result = lua.safe_script(code, &sol::script_pass_on_error);

    if (!result.valid()) {
        sol::error err = result;
        ConsoleLog::Error("%s", err.what());
    }
}