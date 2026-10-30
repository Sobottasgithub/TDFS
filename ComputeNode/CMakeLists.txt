cmake_minimum_required(VERSION 3.10)
project(compute-node)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
# file(GLOB_RECURSE HEADERS "include/*.h")

find_package(pugixml REQUIRED)

add_executable(compute-node ${SOURCES})

target_link_libraries(compute-node PRIVATE tablog)
target_link_libraries(compute-node PRIVATE ttp2)
target_link_libraries(compute-node PRIVATE tud)
target_link_libraries(compute-node PRIVATE pugixml::pugixml)

install(TARGETS compute-node DESTINATION bin)
