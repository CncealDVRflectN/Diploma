cmake_minimum_required(VERSION 3.8)

include("diploma-make-util.cmake")


string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type_upper)
message(STATUS "Build type: ${_build_type_upper}")


if(_build_type_upper STREQUAL "DEBUG")
    set(VTK_DIR $ENV{VTK_DEBUG})
elseif(_build_type_upper STREQUAL "RELEASE")
    set(VTK_DIR $ENV{VTK_RELEASE})
elseif(_build_type_upper STREQUAL "RELWITHDEBINFO")
    set(VTK_DIR $ENV{VTK_REL_DEB})
else()
    message(WARNING "Unsupported build type")
endif()

message(STATUS "VTK_DIR: ${VTK_DIR}")


find_package(VTK COMPONENTS 
    ChartsCore
    CommonCore
    CommonDataModel
    InteractionStyle
    RenderingContext2D
    RenderingContextOpenGL2
    RenderingCore
    RenderingFreeType
    RenderingGL2PSOpenGL2
    RenderingOpenGL2
    ViewsContext2D)

if(NOT VTK_FOUND)
    message(FATAL_ERROR "Skipping Diploma: ${VTK_NOT_FOUND_MESSAGE}")
    return()
endif()

message(STATUS "VTK_VERSION: ${VTK_VERSION}")
message(STATUS "VTK available components: ${VTK_AVAILABLE_COMPONENTS}")


file(GLOB_RECURSE _proj_sources *.cpp)

add_executable(Diploma ${_proj_sources})

target_include_directories(Diploma PUBLIC plots 
					                      problem 
                                          problem/grids
					                      util)

message(STATUS "VTK libraries: ${VTK_LIBRARIES}")


check_vtk_modules_dependencies(OUTPUT_NAMES _required_vtk_modules 
                               MODULES_NAMES ${VTK_LIBRARIES})

message(STATUS "Required VTK modules: ${_required_vtk_modules}")

prepare_copy_vtk_libs(TARGET Diploma 
                      MODULES_NAMES ${_required_vtk_modules})


if(VTK_VERSION VERSION_LESS "8.90.0")
    include(${VTK_USE_FILE})
    target_link_libraries(Diploma PRIVATE ${VTK_LIBRARIES})
else()
    target_link_libraries(Diploma PRIVATE ${VTK_LIBRARIES})
    vtk_module_autoinit(TARGETS Diploma MODULES ${VTK_LIBRARIES})
endif()
