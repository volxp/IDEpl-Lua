# IDEplLua
> [!WARNING]
> This is ment for educational purposes only! Please do not perform any guidelines violating actions with this tool.
> I am not responsible for any actions / account bans from consequences


##  Description

IDEplLua is a specialized Integrated Development Environment tailored for Luau, the scripting language used in Roblox game development. This IDE offers a unique capability: integrated bytecode swapping, which allows for runtime script execution directly on a custom client. This feature streamlines the development process by enabling immediate testing and debugging within the Roblox environment. Furthermore, IDEplLua provides a robust sandboxing environment for scripts on the local machine, executing them in a self-contained process. This ensures a safe and isolated testing ground, preventing any unintended impact on the system while experimenting with new code or features. With IDEplLua, developers can experience a more efficient and secure Luau development workflow, from coding to testing and deployment.

## Project Structure

```
.
├── IDEplLua
│   ├── Compiler
│   │   ├── Sandbox.cpp
│   │   └── Sandbox.hpp
│   ├── Dependencies
│   │   └── imgui
│   │       ├── TextEditor.cpp
│   │       ├── TextEditor.h
│   │       ├── imconfig.h
│   │       ├── imgui.cpp
│   │       ├── imgui.h
│   │       ├── imgui_demo.cpp
│   │       ├── imgui_draw.cpp
│   │       ├── imgui_impl_dx11.cpp
│   │       ├── imgui_impl_dx11.h
│   │       ├── imgui_impl_win32.cpp
│   │       ├── imgui_impl_win32.h
│   │       ├── imgui_internal.h
│   │       ├── imgui_tables.cpp
│   │       ├── imgui_widgets.cpp
│   │       ├── imstb_rectpack.h
│   │       ├── imstb_textedit.h
│   │       └── imstb_truetype.h
│   ├── IDEplLua.cpp
│   ├── IDEplLua.vcxproj
│   ├── IDEplLua.vcxproj.filters
│   └── Overlay
│       ├── Overlay.cpp
│       └── Overlay.hpp
├── IDEplLua.sln
└── ProcessManager
    ├── Include
    │   ├── Log.hpp
    │   └── ProcessManager.hpp
    ├── ProcessManager.vcxproj
    ├── ProcessManager.vcxproj.filters
    └── Source
        └── ProcessManager.cpp
```
## Images

<p float="left">
  <img src="https://github.com/user-attachments/assets/fed3cf35-23e7-4667-acdd-d84d308e05d2" width="290" />
  <img src="https://github.com/user-attachments/assets/471f15ae-6344-4905-85ec-743c737c05af" width="290" />
  <img src="https://github.com/user-attachments/assets/e6993fc0-ee0a-4654-bcbf-6c0289652751" width="290" />
</p>

##  Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Clone** your fork: `git clone https://github.com/volxp/IDEplLua.git`
3. **Create** a new branch: `git checkout -b feature/your-feature`
4. **Commit** your changes: `git commit -am 'Add some feature'`
5. **Push** to your branch: `git push origin feature/your-feature`
6. **Open** a pull request

