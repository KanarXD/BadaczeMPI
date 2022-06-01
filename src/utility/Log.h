#pragma once

#include <iostream>
#include <sstream>
#include <memory>
#include "../models/ProcessData.h"

#define LOG(...)               Log::LogColored(__VA_ARGS__)
#define LOGDEBUG(...)          //Log::LogColored(__VA_ARGS__)
#define LOGINFO(...)           Log::LogInfo(__VA_ARGS__)
#define LOGSTATE(...)          Log::LogColored(__VA_ARGS__)


class Log {
private:
    static int color;
    static std::shared_ptr<ProcessData> processData;
public:
    static void setProcessData(std::shared_ptr<ProcessData> &data) {
        Log::processData = data;
        Log::color = 31 + data->getProcessId() % 7;
    }

    template<typename ...M>
    static void LogInfo(const M &...message) {
        std::stringstream stream;
        LogMessage(stream, message...);
        printf("[INFO] %s \n", stream.str().c_str());
    }

    template<typename ...M>
    static void LogColored(const M &...message) {
        std::stringstream stream;
        LogMessage(stream, message...);
        printf("\033[0;%dm[id: %d][clock: %d][state: %s] %s \033[0;37m\n", color, processData->getProcessId(),
               processData->getClock(), getResourceName(processData->getProcessState()).c_str(), stream.str().c_str());
    }

private:
    template<typename M>
    static void LogMessage(std::ostream &stream, const M &message) {
        stream << message;
    }

    template<typename M, typename ...A>
    static void LogMessage(std::ostream &stream, const M &message, const A &...args) {
        stream << message;
        LogMessage(stream, args...);
    }

    static std::string getResourceName(ProcessState state) {
        switch (state) {
            case IN_GROUP:
                return "IN_GROUP";
            case REQUESTING_GROUP:
                return "REQUESTING_GROUP";
            case REQUESTING_UNR:
                return "REQUESTING_UNR";
            case SLEEPING:
                return "SLEEPING";
        }
    }

};
