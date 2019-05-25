cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE proj_sources *.cpp)

add_executable (Diploma ${proj_sources})
