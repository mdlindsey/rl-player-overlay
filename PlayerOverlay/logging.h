#pragma once
#include <fstream>
#include <iostream>
#include "global.h"

using namespace std;

namespace AnsiText
{
    string Bold   = "\x1b[1m" ;
    string Red    = "\x1b[91m";
    string Green  = "\x1b[92m";
    string Yellow = "\x1b[93m";
    string Reset  = "\x1b[0m" ;
};

class Log {
    public:
    enum class Level { Info, Warn, Error };
    static void Info(string payload, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {
        Log::Handler(payload, Log::Level::Info, pFile, pLine);
    }
    static void Warn(string payload, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {
        Log::Handler(payload, Log::Level::Warn, pFile, pLine);
    }
    static void Error(string payload, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {
        Log::Handler(payload, Log::Level::Error, pFile, pLine);
    }

    static void SetVerbose(bool verbose, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {

    }
    static void SetPrintLevel(Level level, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {
        Log::PRINT_LEVEL = level;
    }
    static void SetWriteLevel(Level level, const char* pFile=__builtin_FILE(), int pLine=__builtin_LINE())
    {
        Log::WRITE_LEVEL = level;
    }

    private:
    inline static bool VERBOSE = false;
    inline static Level PRINT_LEVEL = Log::Level::Info;
    inline static Level WRITE_LEVEL = Log::Level::Warn;
    inline static string WRITE_FILEPATH = "a1m.log";
    static void Handler(string payload, Level level, string pFile, int pLine)
    {
        string prefix = !Log::VERBOSE ? "" : "[" + pFile + ":" + to_string(pLine) + "]\n";
        if (level >= Log::PRINT_LEVEL)
        {
            Log::PrintLog(prefix + payload, level);
        }
        if (level >= Log::WRITE_LEVEL)
        {
            Log::WriteLog(prefix + payload, level);
        }
    }
    static void PrintLog(string payload, Level level)
    {
        string colorAdjustment = "";
        if (level == Log::Level::Error) colorAdjustment = AnsiText::Red;
        if (level == Log::Level::Info)  colorAdjustment = AnsiText::Green;
        if (level == Log::Level::Warn)  colorAdjustment = AnsiText::Yellow;
        Global::CvarManager->log(AnsiText::Bold + colorAdjustment + payload + AnsiText::Reset);
    }
    static void WriteLog(string payload, Level level)
    {
        string linePrefix = "";
        if (level == Log::Level::Info)  linePrefix = "[#]";
        if (level == Log::Level::Warn)  linePrefix = "[!]";
        if (level == Log::Level::Error) linePrefix = "[X]";
        std::ofstream stream(Global::GameWrapper->GetDataFolder() / Log::WRITE_FILEPATH, ios_base::app);
        stream << linePrefix + " " + payload + "\n";
    }
};
