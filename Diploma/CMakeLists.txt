cmake_minimum_required (VERSION 3.8)


file(GLOB_RECURSE _proj_sources *.cpp)

add_executable (Diploma ${_proj_sources})

target_include_directories(Diploma PUBLIC plots 
					                      problem 
                                          problem/grids
					                      util)
