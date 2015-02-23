#ifndef __LSRM_NEIGHBORHOOD_H
#define __LSRM_NEIGHBORHOOD_H

enum CONNECTIVITY{FOUR = 0, EIGHT};

namespace lsrm
{
	void FOURNeighborhood(long int * neighborhood,
						  const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height);
	
	void EIGHTNeighborhood(long int * neighborhood,
						   const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height);
} // end of namespace lsrm

#endif
