#include "Log.h"

Log::Color Log::currentColor = 0;

void Log::LogType(std::ostream& stream, const std::string& logType, const Color& color)
{
    if (currentColor != color)
    {
        stream << "\033[0;" << std::to_string(color) << "m";
        currentColor = color;
    }
    stream << logType << ' ';
}
