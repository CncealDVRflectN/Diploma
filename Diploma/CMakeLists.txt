cmake_minimum_required (VERSION 3.8)


find_package(Gnuplot)

if (NOT GNUPLOT_FOUND)
    message(FATAL_ERROR "Skipping Diploma: Gnuplot not found")
    return()
else()
    message(STATUS "Gnuplot version: ${GNUPLOT_VERSION_STRING}")
endif()

if (GNUPLOT_VERSION_STRING VERSION_LESS "5.0.0")
    message(WARNING "Unsupported Gnuplot version. Only Gnuplot 5.0.0 and higher is currently supported")
endif()


file(GLOB_RECURSE _proj_sources *.cpp)

add_executable (Diploma ${_proj_sources})

target_include_directories(Diploma PUBLIC plots 
					                      problem 
                                          problem/grids
					                      util)


add_custom_command(TARGET Diploma POST_BUILD 
                   COMMAND ${CMAKE_COMMAND} -E copy_directory 
                   ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Diploma>/resources)
