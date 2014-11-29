PROJECT(GRM)

cmake_minimum_required(VERSION 2.8)

FIND_PACKAGE(OTB)
IF(OTB_FOUND)
  INCLUDE(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  MESSAGE(FATAL_ERROR
    "Cannot build OTB project without OTB. Please set OTB_DIR.")
ENDIF(OTB_FOUND)

#Check compiler version
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

file(
	GLOB_RECURSE
	HEADERS
	"src/*.h"
)

file(
	GLOB_RECURSE
	TEMPLATES
	"src/*.txx"
)

file(
	GLOB_RECURSE
	SOURCES
	"src/*.cxx"
)

set(PROJ_INCLUDE_DIRS "")
foreach(_headerFile ${HEADERS})
	get_filename_component(_dir ${_headerFile} PATH)
	list(APPEND PROJ_INCLUDE_DIRS ${_dir})
endforeach()

foreach(_templateFile ${TEMPLATES})
	get_filename_component(_dir ${_templateFile} PATH)
	list(APPEND PROJ_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES PROJ_INCLUDE_DIRS)

include_directories(${PROJ_INCLUDE_DIRS})

ADD_EXECUTABLE(
 	EuclideanDistanceSegmentation
    apps/EuclideanDistanceSegmentation.cxx
	${SOURCES}
)

TARGET_LINK_LIBRARIES(EuclideanDistanceSegmentation OTBCommon OTBIO boost_program_options)

ADD_EXECUTABLE(
	FLSASegmentation
	apps/FLSASegmentation.cxx
	${SOURCES}
)

TARGET_LINK_LIBRARIES(FLSASegmentation OTBCommon OTBIO boost_program_options)

ADD_EXECUTABLE(
	BaatzSegmentation
	apps/BaatzSegmentation.cxx
	${SOURCES}
)

TARGET_LINK_LIBRARIES(BaatzSegmentation OTBCommon OTBIO boost_program_options)
