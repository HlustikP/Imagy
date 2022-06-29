cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)

project(libwebp_test)

# Windows specific stuff
if(WIN32) # Works for both 32 and 64 bit systems btw
	# Define project as a console application
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
	
	# Only available for CMAKE 3.6.0 and higher	
	if(NOT(CMAKE_MAJOR_VERSION LESS 3))
		if(NOT(CMAKE_MINOR_VERSION LESS 6))
			# Tell Visual Studio to use our project as default startup
			set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT libwebp_test)
		endif()
	endif()
endif()

message( STATUS ${CMAKE_CURRENT_SOURCE_DIR} )

execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/test.sh
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)
