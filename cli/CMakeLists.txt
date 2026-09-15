cmake_minimum_required(VERSION 3.10)
project(tfds-cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

add_executable(tdfs-cli ${SOURCES})

target_link_libraries(tdfs-cli PRIVATE tdfs)
target_link_libraries(tdfs-cli PRIVATE tablog)

install(TARGETS tdfs-cli DESTINATION bin)
