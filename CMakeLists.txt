cmake_minimum_required(VERSION 3.19)
project(AVXLib)

set(CMAKE_CXX_STANDARD 17)
include_directories(BEFORE "./")
include_directories(AFTER "C:/Users/kevin/source/C/XVMem")

add_library(AVXLib avx.cpp search.cpp
			avx.h CLexIndex.cpp CLexIndex.h fivebitencoding.cpp fivebitencoding.h
			Searchrequest.h SearchControls.h SearchClause.h SearchFragment.h
			FeatureSpec.h FeatureMatch.h Feature.h)