cmake_minimum_required(VERSION 3.16)
project(lisan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 14 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable((lisan src/main.cpp))

llvm_map_components_to_libnames(LLVM_LIBS core support passes native)
target_link_libraries(lisan PRIVATE ${LLVM_LIBS})