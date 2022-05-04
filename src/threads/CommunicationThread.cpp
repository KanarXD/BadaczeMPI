#include "CommunicationThread.h"
#include "../models/Message.h"
#include "../pch.h"

CommunicationThread::CommunicationThread(std::shared_ptr<ProcessData> &processData) : BaseThread(processData) {

}

void CommunicationThread::Start() {
    setRunning(true);
    communicationThread = std::thread(&CommunicationThread::HandleCommunication, this);
}

void CommunicationThread::Stop() {
    setRunning(false);
    communicationThread.join();
}

void CommunicationThread::HandleCommunication() {
    while (isRunning()) {

        Message message{};
        MPI_Status status;
        MPI_Recv(&message, sizeof(Message), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        LOGINFO("Message: ", message);
        getProcessData()->setClock(message.clock);

    }
}






