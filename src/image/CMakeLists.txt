project(image)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

ADD_LIBRARY( image STATIC
             image.cpp)