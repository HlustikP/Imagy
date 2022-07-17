cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)

project(imagy_test)

if(WIN32)
    set(gtest_force_shared_crt)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG release-1.12.1
    )

    FetchContent_MakeAvailable(googletest)
endif()

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

if(MSVC)
    #set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

if(UNIX)
    SET(CMAKE_THREAD_LIBS_INIT "-lpthread") #-------------|
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") #--|
    SET(CMAKE_HAVE_THREADS_LIBRARY 1) #-------------------|-- Thread management stuff for linux
    SET(CMAKE_USE_WIN32_THREADS_INIT 0) #-----------------|
    SET(CMAKE_USE_PTHREADS_INIT 1) #----------------------|
    SET(THREADS_PREFER_PTHREAD_FLAG ON) #-----------------|

    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory(src)

if(WIN32)
    add_subdirectory(tests)
endif()
