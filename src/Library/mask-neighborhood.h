/*=========================================================================

  Program: Computation of the neighborhood
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __MASKNEIGHBORHOOD_H
#define __MASKNEIGHBORHOOD_H

#include <array>

namespace mask
{
// Different masks for the neighborhood

//     *
//    *X*
//     *

std::array<long int, 4> CrossNeighborhood(long unsigned int coord, unsigned int sizex, unsigned int sizey);

//    ***
//    *X*
//    ***

std::array<long int, 8> SquareNeighborhood(long unsigned int coord, unsigned int sizex, unsigned int sizey);
}// end of namespace lss

#endif
