cmake_minimum_required(VERSION 3.19)
project(AVXLib)

set(CMAKE_CXX_STANDARD 17)
include_directories(AFTER "C:/Users/kevin/source/C/XVMem")

add_library(AVXLib avx.cpp avx.h CLexIndex.cpp CLexIndex.h fivebitencoding.cpp fivebitencoding.h)