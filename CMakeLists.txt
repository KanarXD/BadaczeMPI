cmake_minimum_required(VERSION 3.16)

project(BadaczeMPI)
add_definitions(-D_DEBUG)
set(CMAKE_CXX_STANDARD 20)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -pedantic -lm -O3 -funroll-loops")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(app src/main.cpp src/threads/CommunicationThread.cpp src/threads/CommunicationThread.h src/threads/MainThread.cpp src/threads/MainThread.h src/utility/Log.h src/utility/Log.cpp src/models/ProcessData.cpp src/models/ProcessData.h src/models/Settings.h src/models/Message.h src/threads/BaseThread.cpp src/threads/BaseThread.h src/enums/ResourceType.h src/enums/MessageType.h src/enums/ProcessState.h src/utility/Functions.h)

# OpenMP
find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(app PUBLIC OpenMP::OpenMP_CXX)
endif ()

# MPI
find_package(MPI REQUIRED)
target_link_libraries(app PUBLIC MPI::MPI_CXX)