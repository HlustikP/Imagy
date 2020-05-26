include_directories(${PROJECT_SOURCE_DIR}/src/headers
					)
add_executable(libwebp_test main.cpp) #main entry file

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/src/libs")

add_subdirectory(utils)
add_subdirectory(image)
			  
# TODO: change this to make it first look for the libs in standard dirs for cross-platform compatibility
target_link_libraries(libwebp_test 	${CMAKE_SOURCE_DIR}/src/libs/libwebp.lib
					${CMAKE_SOURCE_DIR}/src/libs/libwebpdecoder.lib
					${CMAKE_SOURCE_DIR}/src/libs/libwebpdemux.lib
					${CMAKE_SOURCE_DIR}/src/libs/Release/utils.lib
					${CMAKE_SOURCE_DIR}/src/libs/imagedec.lib
					${CMAKE_SOURCE_DIR}/src/libs/Release/image.lib
			)