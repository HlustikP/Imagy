function(check_lib lib_name var_name env_name file_name header_only)
	# First we check for the file inside the project's libs folder
	if(EXISTS "${CMAKE_SOURCE_DIR}/src/libs/${PRESET}/${file_name}.lib" OR (header_only AND EXISTS "${CMAKE_SOURCE_DIR}/src/headers/${file_name}"))
		if(NOT header_only)
			set(${var_name} "${CMAKE_SOURCE_DIR}/src/libs/${PRESET}" PARENT_SCOPE)
			message(STATUS "${lib_name} found at ${CMAKE_SOURCE_DIR}/src/libs/${PRESET}/${file_name}.lib")
		else()
			message(STATUS "${lib_name} header-only found at ${CMAKE_SOURCE_DIR}/src/headers/${file_name}")
		endif()
	# If it is not found, we check for an environment variable
	elseif(DEFINED ENV{${env_name}})
		if(NOT header_only)
			set(${var_name} "$ENV{${env_name}}" PARENT_SCOPE)
			message(STATUS "${lib_name} found at $ENV{${env_name}}/${file_name}.lib via environment variable ${env_name}")
		else()
			message(STATUS "${lib_name} header-only found at $ENV{${env_name}} via environment variable ${env_name}")
		endif()
		#include_directories($ENV{${env_name}})
	else()
		message(SEND_ERROR "${lib_name} not found")
	endif()
endfunction()

if(WIN32)
    check_lib("ZLIB" "zlib_dir" "ZLIB_INCLUDE" "zlib" FALSE)
    #check_lib("Boost" "boost_dir" "BOOST_ROOT" "boost" TRUE)
    check_lib("JPEGLIB" "jpeglib_dir" "JPEGLIB_INCLUDE" "jpeg-static" FALSE)
    check_lib("LIBPNG" "libpng_dir" "LIBPNG_INCLUDE" "libpng16" FALSE)
    check_lib("libwebp" "libwebp_dir" "LIBWEBP_INCLUDE" "libwebp" FALSE)
    check_lib("libtiff" "libtiff_dir" "LIBTIFF_INCLUDE" "tiff" FALSE)
    check_lib("libavif" "libavif_dir" "LIBAVIF_INCLUDE" "avif" FALSE)
endif()

if(UNIX)
    #find_package(ZLIB REQUIRED)
    #find_package(PNG REQUIRED)
    list(APPEND CMAKE_PREFIX_PATH "/opt/libjpeg-turbo/lib64/cmake/libjpeg-turbo")
    find_package(libjpeg-turbo CONFIG REQUIRED)
    find_library(WEBP_LIBRARY NAMES webp)
    find_library(WEBP_DEMUX_LIBRARY NAMES webpdemux)
    find_library(WEBP_MUX_LIBRARY NAMES webpmux)
endif()

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/libs/${PRESET}")

add_library(utils STATIC)
add_library(image STATIC)

#set_target_properties(utils PROPERTIES PREFIX "")
#set_target_properties(image PROPERTIES PREFIX "")

include_directories(${PROJECT_SOURCE_DIR}/src/headers)

add_subdirectory(utils)
add_subdirectory(image)
		
add_executable(imagy_test) #defining a main entry file for testing purposes
target_sources(imagy_test PRIVATE "${CMAKE_SOURCE_DIR}/src/main.cpp")
			  
target_link_libraries(imagy_test 	
					utils
					image
			)

if(WIN32)
    set(WIN_LIBS ${zlib_dir}/zlib.lib
        ${libpng_dir}/libpng16.lib
        "${jpeglib_dir}/jpeg-static.lib"
        ${libwebp_dir}/libwebp.lib
		    ${libwebp_dir}/libwebpmux.lib
		    ${libwebp_dir}/libwebpdemux.lib
        ${libtiff_dir}/tiff.lib
        ${libavif_dir}/avif.lib
        ${libavif_dir}/libgav1.lib
        ${libavif_dir}/absl.lib
        ${libavif_dir}/SvtAv1Enc.lib
    )
    set(WIN_LIBS ${WIN_LIBS} PARENT_SCOPE)

    #target_link_libraries(imagy_test ${WIN_LIBS})
    target_link_libraries(image ${WIN_LIBS})
endif()

if(UNIX)
    target_link_libraries(image
        "/usr/local/lib/libpng16.a"
        "/opt/libjpeg-turbo/lib64/libjpeg.a"
        "/usr/local/lib/libwebpmux.a"
        "/usr/local/lib/libwebpdemux.a"
        "/usr/local/lib/libwebp.a"
        "/usr/local/lib/libsharpyuv.a"
        "/usr/local/lib/libtiff.a"
        "/usr/local/lib/libz.a"
        "/usr/local/lib/libavif.a"
        "${CMAKE_SOURCE_DIR}/src/libs/linux-release/libgav1.a"
    )
endif()

install(TARGETS utils image ARCHIVE DESTINATION lib)
