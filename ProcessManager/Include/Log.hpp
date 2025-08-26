#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cstdarg>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

#undef ERROR

class Log {
public:
    enum class Level {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static void info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log(Level::INFO, fmt, args);
        va_end(args);
    }

    static void warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log(Level::WARNING, fmt, args);
        va_end(args);
    }

    static void error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log(Level::ERROR, fmt, args);
        va_end(args);
    }

    static void debug(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log(Level::DEBUG, fmt, args);
        va_end(args);
    }

private:
    static bool enableWindowsConsoleColors() {
#ifdef _WIN32
        static bool initialized = false;
        static bool supportsAnsi = false;

        if (!initialized) {
            initialized = true;

            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) {
                return false;
            }

            if (!_isatty(_fileno(stdout))) {
                return false;
            }

            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (SetConsoleMode(hOut, dwMode)) {
                    supportsAnsi = true;
                }
            }
        }

        return supportsAnsi;
#else
        return true;
#endif
    }

    static void setWindowsConsoleColor(Level level) {
#ifdef _WIN32
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) return;

        WORD color;
        switch (level) {
        case Level::INFO:    color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case Level::WARNING: color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        case Level::ERROR:   color = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
        case Level::DEBUG:   color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
        }
        SetConsoleTextAttribute(hConsole, color);
#endif
    }

    static void resetWindowsConsoleColor() {
#ifdef _WIN32
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) return;

        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    }

    static void setWindowsConsoleColorGray() {
#ifdef _WIN32
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE) return;

        SetConsoleTextAttribute(hConsole, FOREGROUND_INTENSITY);
#endif
    }

    static void log(Level level, const char* fmt, va_list args) {
        constexpr size_t BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        vsnprintf(buffer, BUFFER_SIZE, fmt, args);

        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif

        std::string lvl;
        bool useAnsiColors = enableWindowsConsoleColors();

        switch (level) {
        case Level::INFO:    lvl = "INFO"; break;
        case Level::WARNING: lvl = "WARN"; break;
        case Level::ERROR:   lvl = "ERROR"; break;
        case Level::DEBUG:   lvl = "DEBUG"; break;
        }

        std::cout << "[";

        if (useAnsiColors) {
            std::cout << "\033[90m";
        }
        else {
#ifdef _WIN32
            setWindowsConsoleColorGray();
#endif
        }

        std::cout << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (useAnsiColors) {
            std::cout << "\033[0m";
        }
        else {
#ifdef _WIN32
            resetWindowsConsoleColor();
#endif
        }

        std::cout << "][";

        if (useAnsiColors) {
            switch (level) {
            case Level::INFO:    std::cout << "\033[32m"; break; // Green
            case Level::WARNING: std::cout << "\033[33m"; break; // Yellow
            case Level::ERROR:   std::cout << "\033[31m"; break; // Red
            case Level::DEBUG:   std::cout << "\033[36m"; break; // Cyan
            }
        }
        else {
#ifdef _WIN32
            setWindowsConsoleColor(level);
#endif
        }

        std::cout << lvl;

        if (useAnsiColors) {
            std::cout << "\033[0m";
        }
        else {
#ifdef _WIN32
            resetWindowsConsoleColor();
#endif
        }

        std::cout << "] " << buffer << std::endl;
    }
};