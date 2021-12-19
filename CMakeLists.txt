cmake_minimum_required(VERSION 3.21)
project(HPCFinal C)

set(CMAKE_C_STANDARD 11)
include_directories(/usr/lib/x86_64-linux-gnu/openmpi/include)
add_executable(HPCFinal main.c lib/csvReader.h lib/kmeans.c lib/kmeans.h mpi/main_mpi.c cuda/main.cu cuda/test.cu)
target_link_libraries(HPCFinal m)
