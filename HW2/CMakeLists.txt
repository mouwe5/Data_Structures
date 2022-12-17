cmake_minimum_required(VERSION 3.16)
project(Game VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

# If you want to see "under the hood" of CMake, uncomment this:
set(CMAKE_VERBOSE_MAKEFILE ON)

# Define variables containing compiler flags for gcc
set(MTM_FLAGS_DEBUG "-std=c++11 --pedantic-errors -Wall -Werror")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")

# Set the flags for gcc
set(CMAKE_CPP_FLAGS ${MTM_FLAGS_DEBUG})
set(CPP_FILES

        library2.h library2.cpp
        AvlTree.h DynamicArray.h
		KeyByLevel.cpp KeyByLevel.h Key.h
		GM.cpp GM.h
        Player.h Player.cpp
        UnionFindGroups.cpp UnionFindGroups.h)

add_executable(GameExe main2.cpp ${CPP_FILES})
