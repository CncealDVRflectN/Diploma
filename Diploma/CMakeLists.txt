cmake_minimum_required (VERSION 3.8)

include_directories(plot 
					problem 
					util)

file(GLOB_RECURSE proj_sources *.cpp)

add_executable (Diploma ${proj_sources})
