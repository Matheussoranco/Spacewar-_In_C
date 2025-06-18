cmake_minimum_required(VERSION 3.31)
project(Spacewar__In_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Spacewar__In_C
        classic_way_spacewar.c
        spacewar.c)
