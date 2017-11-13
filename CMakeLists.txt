CMAKE_MINIMUM_REQUIRED (VERSION 2.9)
SET(CMAKE_VERBOSE_MAKEFILE 1)

PROJECT(mi4)
set(MI4_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include")
include_directories (${MI4_INCLUDE_DIRS})

#packages
find_package(OpenGL)
find_package(glfw3)
find_package(GLEW)

if (glfw3_FOUND)
    include_directories(${GLFW3_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GLFW3_LIB_DIRS})
endif()

if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    LINK_DIRECTORIES(${GLEW_LIB_DIRS})
endif()

find_package(Eigen3)
if (Eigen3_FOUND)
	include_directories (${EIGEN3_INCLUDE_DIRS})
else ()
	include_directories ("c:/usr/local/include")
endif (Eigen3_FOUND)


ENABLE_TESTING()

SET ( CMAKE_MACOSX_RPATH 1 )
set ( CMAKE_BUILD_TYPE Release )
if (WIN32)
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
else () # Cygwin, Mac
	add_definitions(-g -O3 -Wall -std=c++11 -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic)
	#set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wvariadic-macros -Wwrite-strings ")
		 #	add_definitions(-g -O3 -Wall -std=c++11 -Werror)
	link_directories(/usr/local/lib)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -pg")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
endif (WIN32)
ADD_SUBDIRECTORY(include/mi4)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)

