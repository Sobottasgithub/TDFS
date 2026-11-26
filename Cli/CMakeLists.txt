cmake_minimum_required(VERSION 3.10)
project(tfds-cli)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "./*.cpp" "./*.h")

find_package(PkgConfig REQUIRED)
pkg_check_modules(READLINE REQUIRED readline)
find_package(Arrow REQUIRED)
find_package(Parquet REQUIRED)

add_executable(tdfs-cli ${SOURCES})

target_include_directories(tdfs-cli PRIVATE ${READLINE_INCLUDE_DIRS})
target_link_directories(tdfs-cli PRIVATE ${READLINE_LIBRARY_DIRS})

target_link_libraries(tdfs-cli PRIVATE 
    ${READLINE_LIBRARIES}
    tablog
    ttp2
)
target_link_libraries(tdfs-cli PRIVATE arrow_shared)
target_link_libraries(tdfs-cli PRIVATE parquet_shared)

install(TARGETS tdfs-cli DESTINATION bin)
