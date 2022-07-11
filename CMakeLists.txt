cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

project(imagy_test)

if (MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)
