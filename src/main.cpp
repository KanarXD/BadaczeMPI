//
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "utility/Log.h"
#include "models.h"


#define ARG_COUNT 5


void check_thread_support(int);

int main(int argCount, char **args) {

    if (argCount < ARG_COUNT) {
        LOGERROR("Too few arguments, args num: ", argCount);
        exit(-1);
    }

    int provided;
    MPI_Init_thread(&argCount, &args, MPI_THREAD_MULTIPLE, &provided);

    check_thread_support(provided);

    Settings settings = {atoi(args[1]), atoi(args[2]), atoi(args[3]), atoi(args[4])};
    LOGINFO("Settings: ", settings);

    ProcessData processData(1);
    MPI_Comm_rank(MPI_COMM_WORLD, &processData.processId);
    MPI_Comm_size(MPI_COMM_WORLD, &processData.processCount);

    LOGINFO("ThreadId: ", processData.processId);

    sleep(1);

    return 0;
}


void check_thread_support(int provided) {
    LOGINFO("THREAD SUPPORT: chcemy: ", provided, ". Co otrzymamy?\n");
    switch (provided) {
        case MPI_THREAD_SINGLE:
            LOGINFO("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
            LOGINFO(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
            MPI_Finalize();
            exit(-1);
            break;
        case MPI_THREAD_FUNNELED:
            LOGINFO("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
            break;
        case MPI_THREAD_SERIALIZED:
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            LOGINFO("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
            break;
        case MPI_THREAD_MULTIPLE:
            LOGINFO("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
            break;
        default:
            LOGINFO("Nikt nic nie wie\n");
    }
}
