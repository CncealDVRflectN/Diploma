cmake_minimum_required(VERSION 3.8)


function(check_vtk_modules_dependencies)
    cmake_parse_arguments(_diploma_check 
                          "" 
                          "" 
                          "OUTPUT_NAMES;MODULES_NAMES;IGNORE_MODULES" 
                          ${ARGN})

    if(_diploma_check_UNPARSED_ARGUMENTS)
        message(WARNING 
                "Unparsed arguments for 'check_vtk_modules_dependecies': 
                ${_diploma_check_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT _diploma_check_OUTPUT_NAMES)
        message(FATAL_ERROR "The 'OUTPUT_NAMES' argument is required.")
    endif()

    if (NOT _diploma_check_MODULES_NAMES)
        message(WARNING "The 'MODULES_NAME' argument is not set.")
    endif()

    string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type_upper)

    set(_result "")
    foreach(_module_name ${_diploma_check_MODULES_NAMES})
        if((NOT _module_name IN_LIST _result) AND (NOT _module_name IN_LIST _diploma_check_IGNORE_MODULES))
            list(APPEND _result ${_module_name})

            set(_all_module_dependencies "")
            get_target_property(_lib_type ${_module_name} "TYPE")
            if(NOT _lib_type STREQUAL "INTERFACE_LIBRARY")
                get_target_property(_module_dependencies ${_module_name} "IMPORTED_LINK_DEPENDENT_LIBRARIES_${_build_type_upper}")
                if(_module_dependencies)
                    list(APPEND _all_module_dependencies ${_module_dependencies})
                endif()
            endif()

            get_target_property(_module_dependencies ${_module_name} "INTERFACE_vtk_module_depends")
            if(_module_dependencies)
                list(APPEND _all_module_dependencies ${_module_dependencies})
            endif()

            if(_all_module_dependencies)
                set(_ignore_modules "${_diploma_check_IGNORE_MODULES}")
                list(APPEND _ignore_modules ${_result})

                check_vtk_modules_dependencies(OUTPUT_NAMES _dependencies
                                               MODULES_NAMES ${_all_module_dependencies} 
                                               IGNORE_MODULES ${_ignore_modules})

                list(APPEND _result ${_dependencies})
            endif()
        endif()
    endforeach()

    set("${_diploma_check_OUTPUT_NAMES}" ${_result} PARENT_SCOPE)
endfunction(check_vtk_modules_dependencies)


function(prepare_copy_vtk_libs)
    cmake_parse_arguments(_diploma_prepare 
                          "" 
                          "TARGET" 
                          "MODULES_NAMES" 
                          ${ARGN})

    if(NOT _diploma_prepare_TARGET)
        message(FATAL_ERROR "The 'TARGET' argument is required.")
    endif()

    string(TOUPPER ${CMAKE_BUILD_TYPE} _build_type_upper)

    message(STATUS "VTK required libraries paths to copy:")
    foreach(_module_name ${_diploma_prepare_MODULES_NAMES})
        get_target_property(_lib_type ${_module_name} "TYPE")

        if(NOT _lib_type STREQUAL "INTERFACE_LIBRARY")
            get_target_property(_lib_path ${_module_name} "IMPORTED_LOCATION_${_build_type_upper}")
            message(STATUS "    ${_lib_path}")

            if(_lib_path)
                add_custom_command(TARGET ${_diploma_prepare_TARGET} POST_BUILD 
                                   COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                                   ${_lib_path} $<TARGET_FILE_DIR:${_diploma_prepare_TARGET}>)
            endif()
        endif()
    endforeach()
endfunction(prepare_copy_vtk_libs)
