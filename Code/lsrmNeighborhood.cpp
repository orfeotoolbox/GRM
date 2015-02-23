#include "lsrmNeighborhood.h"

namespace lsrm
{
	void FOURNeighborhood(long int * neighborhood,
						  const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height)
	{	
		const unsigned int x = id % width;
		const unsigned int y = id / width;

		/* top */
		neighborhood[0] = ( y > 0 ? (id - width) : -1 );

		/* right */
		neighborhood[1] = ( x < (width - 1) ? (id + 1) : -1 );

		/* bottom */
		neighborhood[2] = ( y < (height - 1) ? (id + width) : -1 );

		/* left */
		neighborhood[3] = ( x > 0 ? (id - 1) : -1 );
	}
	
	void EIGHTNeighborhood(long int * neighborhood,
						   const long unsigned int id,
						  const unsigned int width,
						  const unsigned int height)
	{
		const unsigned int x = id % width;
		const unsigned int y = id / width;

		/* top */
		neighborhood[0] = ( y > 0 ? (id - width) : -1 );

		/* top right */
		neighborhood[1] = ( (y > 0 && x < (width - 1) ) ? (id - width + 1) : -1 );
		
		/* right */
		neighborhood[2] = ( x < (width - 1) ? (id + 1) : -1 );

		/* bottom right */
		neighborhood[3] = ( (x < (width - 1) && y < (height - 1) ) ? (id + 1 + width) : -1);

		/* bottom */
		neighborhood[4] = ( y < (height - 1) ? (id + width) : -1 );

		/* bottom left */
		neighborhood[5] = ( (y < (height - 1) && x > 0) ? (id + width - 1) : -1 );
		
		/* left */
		neighborhood[6] = ( x > 0 ? (id - 1) : -1 );

		/* top left */
		neighborhood[7] = ( (x > 0 && y > 0) ? (id -width - 1) : - 1);
	}
} // end of namespace lsrm
