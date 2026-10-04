cmake_minimum_required(VERSION 3.10)
project(tdfs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(DEF_NETWORKMANAGER "configure networking lib" ON)
option(DEF_TDFS "configure tdfs" ON)
option(DEF_CLI "configure tdfs cli" ON)

if (DEF_NETWORKMANAGER)
    add_subdirectory(lib/libnetworkmanager)
endif()

if(DEF_TDFS)
    add_subdirectory(tdfs)
endif()

# TDFS CARGV client
if(DEF_CLI)
    add_subdirectory(cli)
endif()
