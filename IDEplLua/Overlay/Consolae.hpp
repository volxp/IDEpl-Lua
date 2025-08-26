#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <cstdarg>


enum class LogLevel {
    Default,
    Info,
    Warning,
    Error
};

struct LogEntry {
    std::string Message;
    LogLevel Level;
};

class ConsoleLog {
public:
    static void Log(const char* fmt, ...);
    static void Info(const char* fmt, ...);
    static void Warn(const char* fmt, ...);
    static void Error(const char* fmt, ...);

    static void Draw();

private:
    static void AddLog(LogLevel level, const char* fmt, va_list args);

    static std::vector<LogEntry> logs;
    static std::mutex logMutex;
};
