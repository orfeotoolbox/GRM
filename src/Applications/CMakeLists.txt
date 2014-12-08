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
add_executable(EuclideanDistanceSegmentation EuclideanDistanceSegmentation.cxx)
target_link_libraries(EuclideanDistanceSegmentation 
					GRM)

add_executable(FLSASegmentation FLSASegmentation.cxx)
target_link_libraries(FLSASegmentation 
						GRM)

add_executable(BaatzSegmentation BaatzSegmentation.cxx)
target_link_libraries(BaatzSegmentation 
					GRM)
