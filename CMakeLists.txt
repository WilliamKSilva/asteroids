cmake_minimum_required(VERSION 3.10)

project(Asteroids)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SOURCE_FILES main.c player.c collision_area.c)

add_executable(Asteroids ${SOURCE_FILES})

add_subdirectory(/usr/local/lib/raylib /usr/local/lib/raylib/src)
target_link_libraries(Asteroids raylib)
