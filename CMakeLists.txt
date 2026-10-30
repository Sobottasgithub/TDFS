cmake_minimum_required(VERSION 3.10)
project(tdfs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DEF_LIB "configure lib" ON)
option(DEF_COMPUTE_NODE "configure compute node" ON)
option(DEF_CLI "configure tdfs cli" ON)

if (DEF_LIB)
    add_subdirectory(lib/)
endif()

if(DEF_COMPUTE_NODE)
    add_subdirectory(ComputeNode)
endif()

if(DEF_NAME_SERVER)
    add_subdirectory(NameServer)
endif()

# TDFS CARGV client
if(DEF_CLI)
    add_subdirectory(Cli)
endif()
