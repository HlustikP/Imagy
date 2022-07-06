project(utils)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

ADD_LIBRARY( utils STATIC
			gif_decode.cpp
            file_io.cpp )