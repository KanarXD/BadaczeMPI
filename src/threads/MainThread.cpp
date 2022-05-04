
#include "MainThread.h"
#include "../pch.h"
#include "../models/Message.h"

#define MIN_SLEEP_TIME 200
#define MAX_SLEEP_TIME 2000

MainThread::MainThread(std::shared_ptr<ProcessData> &processData) : BaseThread(processData),
                                                                    communicationThread(processData) {
    std::srand(processData->getProcessId());
}

void MainThread::Start() {
    setRunning(true);
    communicationThread.Start();
    while (isRunning()) {
        getProcessData()->increaseClock();
        Message message{getProcessData()->getProcessId(), getProcessData()->getClock()};
        MPI_Send(&message, sizeof(Message), MPI_BYTE, rand() % getProcessData()->getProcessCount(), 0,
                 MPI_COMM_WORLD);
        sleep();
    }
    communicationThread.Stop();
}

void MainThread::sleep() {
    std::this_thread::sleep_for(
            std::chrono::milliseconds(MIN_SLEEP_TIME + rand() % (MAX_SLEEP_TIME - MIN_SLEEP_TIME)));
}
