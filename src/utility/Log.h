#pragma once

#include <iostream>


#ifdef _DEBUG
#define LOGERROR(...)		Log::Error(__VA_ARGS__)
#define LOGINFO(...)		Log::Info(__VA_ARGS__)
#define LOGWARNING(...)		Log::Warning(__VA_ARGS__)
#define LOGTRACE(...)		Log::Trace(__VA_ARGS__)
#else
#define LOGERROR(...)		Log::Error(__VA_ARGS__)
#define LOGINFO(...)
#define LOGWARNING(...)
#define LOGTRACE(...)
#endif

class Log
{
private:
    using Color = unsigned short;
    enum : Color { Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37 };
private:
    static Color currentColor;
public:

    template<typename ...M>
    static void Error(const M& ...message)
    {
        LogType(std::cerr, "|ERROR|", Red);
        LogMessage(std::cerr, message...);
    }

    template<typename ...M>
    static void Info(const M& ...message)
    {
        LogType(std::cout, "|INFO|", Yellow);
        LogMessage(std::cout, message...);
    }

    template<typename ...M>
    static void Warning(const M& ...message)
    {
        LogType(std::cout, "|WARNING|", Magenta);
        LogMessage(std::cout, message...);
    }

    template<typename ...M>
    static void Trace(const M& ...message)
    {
        LogType(std::cout, "|TRACE|", Blue);
        LogMessage(std::cout, message...);
    }

private:
    static void LogType(std::ostream& stream, const std::string& logType, const Color& color);

    template<typename M>
    static void LogMessage(std::ostream& stream, const M& message)
    {
        stream << message << std::endl;
    }

    template<typename M, typename ...A>
    static void LogMessage(std::ostream& stream, const M& message, const A& ...args)
    {
        stream << message;
        LogMessage(stream, args...);
    }

};
