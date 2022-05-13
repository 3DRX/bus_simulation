
cmake_minimum_required (VERSION 3.10)

project (Bus-operation)

aux_source_directory(./src DIR_SRCS)

add_executable(out ${DIR_SRCS})

