CMAKE_MINIMUM_REQUIRED (VERSION 2.9)
SET(CMAKE_VERBOSE_MAKEFILE 1)
PROJECT(mi4)
#linux
if (UNIX AND NOT APPLE)
	set(CMAKE_MODULE_PATH /usr/local/lib/cmake)
	SET (LINUX TRUE)
endif()

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
endif (Eigen3_FOUND)



ENABLE_TESTING()
if (WIN32)

else () # Cygwin, Mac
	#add_definitions(-g -O3 -Wall -std=c++11 -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic)
	add_definitions(-g -O3 -Wall -std=c++11 -Werror)
	link_directories(/usr/local/lib)
endif (WIN32)
ADD_SUBDIRECTORY(include/mi4)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(dev)
