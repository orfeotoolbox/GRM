#ifndef __LSRM_DATA_STRUCTURES_H
#define __LSRM_DATA_STRUCTURES_H
#include <bitset>
#include <vector>

namespace lsrm
{	
	typedef std::bitset<2> ContourElem;
	typedef std::vector<ContourElem> Contour;
	typedef typename Contour::iterator ContourIterator;
	typedef typename Contour::const_iterator ContourConstIterator;

	struct BoundingBox
	{
		/* X coordinate of the upper left. */
		long unsigned int m_UX;
		
		/* Y coordinate of the upper left. */
		long unsigned int m_UY;
		
		/* Width */
		unsigned int m_W;

		/* Height */
		unsigned int m_H;	
	};
	
} // end of namespace lsrm

#endif
