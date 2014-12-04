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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Library)

add_subdirectory(Library)
add_subdirectory(Applications)
