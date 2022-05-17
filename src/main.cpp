#include <mpi.h>
#include "threads/MainThread.h"
#include "utility/Log.h"
#include "models/Settings.h"

#define ARG_COUNT 4

void check_thread_support(int);

int main(int argCount, char **args) {

    if (argCount < ARG_COUNT) {
        LOGINFO("Too few arguments, args num: ", argCount);
        LOGINFO("Required args count: ", ARG_COUNT - 1, " (groupSize, UNRCount, GroupCount)");
        exit(-1);
    }

    Settings settings = {atoi(args[1]), atoi(args[2]), atoi(args[3])};

    int provided;
    MPI_Init_thread(&argCount, &args, MPI_THREAD_MULTIPLE, &provided);
//    check_thread_support(provided);

    int processId;
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &settings.processCount);

    LOGINFO("Settings: ", settings);

    std::shared_ptr<ProcessData> processData =
            std::make_shared<ProcessData>(processId, settings.groupSize, settings);

    Log::setProcessData(processData);

    LOG("ThreadId: ", processData->getProcessId());

    MainThread mainThread(processData);
    mainThread.Start();

    return 0;
}

//
//void check_thread_support(int provided) {
//    LOGINFO("THREAD SUPPORT: chcemy: ", provided, ". Co otrzymamy?\n");
//    switch (provided) {
//        case MPI_THREAD_SINGLE:
//            LOGINFO("Brak wsparcia dla wątków, kończę\n");
//            /* Nie ma co, trzeba wychodzić */
//            LOGINFO(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
//            MPI_Finalize();
//            exit(-1);
//            break;
//        case MPI_THREAD_FUNNELED:
//            LOGINFO("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
//            break;
//        case MPI_THREAD_SERIALIZED:
//            /* Potrzebne zamki wokół wywołań biblioteki MPI */
//            LOGINFO("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
//            break;
//        case MPI_THREAD_MULTIPLE:
//            LOGINFO("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
//            break;
//        default:
//            LOGINFO("Nikt nic nie wie\n");
//    }
//}
