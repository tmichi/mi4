CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT(mi4 CXX)
# set(CMAKE_VERBOSE_MAKEFILE 1) #コンパイルオプションを表示
# set(CMAKE_INSTALL_PREFIX "./ex")
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)
# set(CMAKE_BUILD_TYPE MinSizeRel)

#
# C++ options
#
set(CMAKE_CXX_STANDARD 14) #11, 17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE) #Apple
        set(CMAKE_MACOSX_RPATH 1)
endif(APPLE)

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

find_package(Eigen3 3.0 REQUIRED)
if (Eigen3_FOUND)
	include_directories (${EIGEN3_INCLUDE_DIRS})
else ()
	#Set Eigen3 folder
	include_directories ("c:/usr/local/include")
endif (Eigen3_FOUND)


ENABLE_TESTING()

if(MSVC) #Visual Studio
        #Use static library
        set(CMAKE_CXX_FLAGS "/W4")
        set(CMAKE_CXX_FLAGS_RELEASE "/MT")
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT") #todo
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT") #todo
elseif(CMAKE_COMPILER_IS_GNUCXX) #UNIX
        INCLUDE_DIRECTORIES(/usr/local/inlcude)
        LINK_DIRECTORIES(/usr/local/lib)
        #共通
        set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization -Wendif-labels -Wfloat-equal -Winit-self -Winline -Wmissing-include-dirs -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow -Wsign-promo -Wswitch-default -Wswitch-enum -Wvariadic-macros -Wwrite-strings ")
        set(CMAKE_CXX_FLAGS_DEBUG          "-g3 -O0 -pg")
        set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG -march=native")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g3 -Og -pg")
        set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG -march=native")
endif()

ADD_SUBDIRECTORY(include/mi4)
ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(examples)

