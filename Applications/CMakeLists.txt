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
add_executable(RegionMergingSegmentation RegionMergingSegmentation.cxx)
target_link_libraries(RegionMergingSegmentation OTBGRM)

add_executable(SpringSegmentation SpringSegmentation.cxx)
target_link_libraries(SpringSegmentation OTBGRM)

add_executable(FullLambdaScheduleSegmentation FullLambdaScheduleSegmentation.cxx)
target_link_libraries(FullLambdaScheduleSegmentation OTBGRM)
