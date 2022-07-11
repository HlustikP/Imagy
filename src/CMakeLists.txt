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
		include_directories($ENV{${env_name}})
	else()
		message(SEND_ERROR "${lib_name} not found")
	endif()
endfunction()

check_lib("ZLIB" "zlib_dir" "ZLIB_INCLUDE" "zlib" FALSE)
check_lib("Boost" "boost_dir" "BOOST_ROOT" "boost" TRUE)
check_lib("JPEGLIB" "jpeglib_dir" "JPEGLIB_INCLUDE" "jpeg" FALSE)
check_lib("LIBPNG" "libpng_dir" "LIBPNG_INCLUDE" "libpng16" FALSE)
check_lib("libwebp" "libwebp_dir" "LIBWEBP_INCLUDE" "libwebp" FALSE)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/libs/${PRESET}")

add_library(utils STATIC)
add_library(image STATIC)

include_directories(${PROJECT_SOURCE_DIR}/src/headers)

add_subdirectory(utils)
add_subdirectory(image)
		
add_executable(imagy_test) #defining a main entry file for testing purposes
target_sources(imagy_test PRIVATE "${CMAKE_SOURCE_DIR}/src/main.cpp")
			  
target_link_libraries(imagy_test 	${libwebp_dir}/libwebp.lib
					${libwebp_dir}/libwebpmux.lib
					${libwebp_dir}/libwebpdemux.lib
					${jpeglib_dir}/jpeg.lib
					${libpng_dir}/libpng16.lib
					utils
					image
			)

if (WIN32)
    target_link_libraries(imagy_test ${zlib_dir}/zlib.lib)
endif

if (UNIX)
    target_link_libraries(imagy_test ZLIB::ZLIB)
endif

install(TARGETS utils image ARCHIVE DESTINATION lib)
