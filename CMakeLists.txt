
cmake_minimum_required (VERSION 2.8)

project (Bus-operation)

aux_source_directory(./src DIR_SRCS)

add_executable(out ${DIR_SRCS})

