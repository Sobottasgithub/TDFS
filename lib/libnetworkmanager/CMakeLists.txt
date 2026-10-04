cmake_minimum_required(VERSION 3.10)
project(networkmanager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
# file(GLOB_RECURSE HEADERS "include/*.h")

add_library(networkmanager SHARED ${SOURCES})

target_include_directories(networkmanager PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(networkmanager PRIVATE tablog)

install(TARGETS networkmanager DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
