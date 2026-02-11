cmake_minimum_required(VERSION 3.30)

set(target comm-daemon)

add_executable(${target})

target_sources(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

add_executable(client ${CMAKE_CURRENT_LIST_DIR}/main.cpp)