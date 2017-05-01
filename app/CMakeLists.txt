cmake_minimum_required(VERSION 3.6)
project(proxmark3 LANGUAGES C)

add_subdirectory(../clib clib)
add_subdirectory(src/main/c)
