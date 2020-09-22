cmake_minimum_required(VERSION 2.8)
project(utils)
ADD_LIBRARY( utils STATIC
			gif_decode.cpp
            file_io.cpp )