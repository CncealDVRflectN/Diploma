﻿cmake_minimum_required (VERSION 3.8)

project ("Diploma")

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                         /W4 \
                         /analyze")

    if(MSVC_VERSION GREATER_EQUAL 1910)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                             /diagnostics:column")
    endif()

	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
                               /Od \
                               /Z7")

	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} \
                                        /O2 \
                                        /GF \
                                        /GL \
                                        /Gw \
                                        /Gy \
                                        /Z7 \
                                        /openmp \
                                        /Qpar \
                                        /Qpar-report:2 \
                                        /Qvec-report:2")

	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
                                 /O2 \
                                 /GF \
                                 /GL \
                                 /Gw \
                                 /Gy \
                                 /Zi \
                                 /openmp \
                                 /Qpar \
                                 /Qpar-report:1 \
                                 /Qvec-report:1")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                         -Wall \
                         -Wextra")

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} \
                               -O0")

    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} \
                                        -O2")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} \
                                 -O3")

    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                             -stdlib=libc++")
    endif()
endif()

add_subdirectory ("Diploma")
