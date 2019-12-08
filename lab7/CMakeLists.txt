cmake_minimum_required(VERSION 3.4)
project(diffie_hellman)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-Wall -O2 -fPIC)

set(ELLIPTIC_SOURCE src/elliptic.cpp src/utils.cpp)
add_library(elliptic_lib STATIC ${ELLIPTIC_SOURCE})

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} elliptic_lib)
