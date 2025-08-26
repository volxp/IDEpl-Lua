#include "Consolae.hpp"
#include <imgui/imgui.h>

std::vector<LogEntry> ConsoleLog::logs;
std::mutex ConsoleLog::logMutex;

void ConsoleLog::AddLog(LogLevel level, const char* fmt, va_list args) {
    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);

    char finalMsg[1050];
    const char* levelStr = "";
    switch (level) {
    case LogLevel::Default:    levelStr = ""; break;
    case LogLevel::Info:    levelStr = "[INFO] "; break;
    case LogLevel::Warning: levelStr = "[WARN] "; break;
    case LogLevel::Error:   levelStr = "[ERROR] "; break;
    }
    snprintf(finalMsg, sizeof(finalMsg), "%s%s", levelStr, buf);

    std::lock_guard<std::mutex> lock(logMutex);
    logs.push_back({ finalMsg, level });
}

void ConsoleLog::Log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddLog(LogLevel::Default, fmt, args);
    va_end(args);
}
void ConsoleLog::Info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddLog(LogLevel::Info, fmt, args);
    va_end(args);
}

void ConsoleLog::Warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddLog(LogLevel::Warning, fmt, args);
    va_end(args);
}

void ConsoleLog::Error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AddLog(LogLevel::Error, fmt, args);
    va_end(args);
}

void ConsoleLog::Draw() {
    ImVec2 console_size = ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 10.5f);

    if (ImGui::BeginChild("ConsoleBox", console_size, true, ImGuiWindowFlags_HorizontalScrollbar)) {
        std::lock_guard<std::mutex> lock(logMutex);

        for (const auto& log : logs) {
            ImVec4 color;
            switch (log.Level) {
            case LogLevel::Default:   color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
            case LogLevel::Error:   color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f); break;
            case LogLevel::Warning: color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f); break;
            case LogLevel::Info:    color = ImVec4(0.6f, 1.0f, 0.6f, 1.0f); break;
            }
            ImGui::TextColored(color, "%s", log.Message.c_str());
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
    }
}
