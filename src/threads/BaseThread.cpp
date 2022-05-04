#include "BaseThread.h"

BaseThread::BaseThread(std::shared_ptr<ProcessData> &processData) : processData(processData) {}

bool BaseThread::isRunning() const {
    return running;
}

void BaseThread::setRunning(bool running) {
    BaseThread::running = running;
}

const std::shared_ptr<ProcessData> &BaseThread::getProcessData() const {
    return processData;
}
