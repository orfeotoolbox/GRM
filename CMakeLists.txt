#=========================================================================

#  Program: Generic Region Merging Library (GRM)
#  Language: C++
#  author: Lassalle Pierre



#  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved

#  See grmlib-copyright.txt for details.

#     This software is distributed WITHOUT ANY WARRANTY; without even
#     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#     PURPOSE.  See the above copyright notices for more information.

#=========================================================================
project(GRM)

cmake_minimum_required(VERSION 2.8)

find_package(OTB)
IF(OTB_FOUND)
  include(${OTB_USE_FILE})
ELSE(OTB_FOUND)
  message(FATAL_ERROR
    "Cannot build OTB project without OTB. Please set OTB_DIR.")
ENDIF(OTB_FOUND)

#Activate c++11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive -Wall -Wmaybe-uninitialized")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Code)

add_subdirectory(Code)
add_subdirectory(Applications)
