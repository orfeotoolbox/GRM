/*=========================================================================

  Program: Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef GRM_NEIGHBORHOOD_H
#define GRM_NEIGHBORHOOD_H

enum CONNECTIVITY{FOUR = 0, EIGHT};

namespace grm
{
	void FOURNeighborhood(long int * neighborhood,
						  const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height);
	
	void EIGHTNeighborhood(long int * neighborhood,
						   const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height);
} // end of namespace grm

#endif
