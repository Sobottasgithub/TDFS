cmake_minimum_required(VERSION 3.10)
project(tdfs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DEF_TDFS "configure library" ON)
option(DEF_CLI "configure tdfs cli" ON)

if(DEF_TDFS)
    add_subdirectory(lib/tdfs)
endif()

# TDFS CARGV client
if(DEF_CLI)
    add_subdirectory(cli)
endif()
