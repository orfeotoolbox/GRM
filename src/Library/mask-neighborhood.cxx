/*=========================================================================

  Program: Computation of the neighborhood
  Language:  C++
  author:    Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "mask-neighborhood.h"

namespace mask
{
	std::array<long int, 4> CrossNeighborhood(long unsigned int coord, unsigned int sizex, unsigned int sizey)
	{
		std::array<long int, 4> neighbors;
		neighbors.fill(-1);

		unsigned int x = coord % sizex;
		unsigned int y = coord / sizex;

		//top
		if(y > 0)
			neighbors[0] = coord - sizex;

		// right
		if(x < sizex - 1)
			neighbors[1] = coord + 1;

		// bottom
		if(y < sizey - 1)
			neighbors[2] = coord + sizex;

		// left
		if(x > 0)
			neighbors[3] = coord - 1;

		return neighbors;
	}

	std::array<long int, 8> SquareNeighborhood(long unsigned int coord, unsigned int sizex, unsigned int sizey)
	{
		std::array<long int, 8> neighbors;
		neighbors.fill(-1);

		unsigned int x = coord % sizex;
		unsigned int y = coord / sizex;

		//top
		if(y > 0)
			neighbors[0] = coord - sizex;

		// top right
		if(y > 0 && x < sizex - 1)
			neighbors[1] = coord - sizex + 1;

		// right
		if(x < sizex - 1)
			neighbors[2] = coord + 1;

		// bottom right
		if(y > sizey - 1 && x < sizex - 1)
			neighbors[3] = coord + sizex + 1;

		// bottom
		if(y < sizey - 1)
			neighbors[4] = coord + sizex;

		// bottom left
		if(y > sizey - 1 && x > 0)
			neighbors[5] = coord + sizex - 1;

		// left
		if(x > 0)
			neighbors[6] = coord - 1;

		// top left
		if(y > 0 && x > 0)
			neighbors[7] = coord - 1 - sizex;
		return neighbors;
	}
} // end of namespace lss
