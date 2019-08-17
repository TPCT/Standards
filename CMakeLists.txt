cmake_minimum_required(VERSION 3.14)
project(Standards C)

set(CMAKE_C_STANDARD 11)

add_executable(Standards main.c  standards.c standards.h)
