cmake_minimum_required(VERSION 3.10)
project(tfds-cli)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

add_executable(tdfs-cli ${SOURCES})

target_include_directories(tdfs-cli PRIVATE ${READLINE_INCLUDE_DIRS})
target_link_directories(tdfs-cli PRIVATE ${READLINE_LIBRARY_DIRS})
target_link_libraries(tdfs-cli PRIVATE ${READLINE_LIBRARIES})

target_link_libraries(tdfs-cli PRIVATE tablog)
target_link_libraries(tdfs-cli PRIVATE ttp2)

install(TARGETS tdfs-cli DESTINATION bin)
