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
#ifndef GRM_DATA_STRUCTURES_H
#define GRM_DATA_STRUCTURES_H
#include <stdexcept>
#include <bitset>
#include <vector>

namespace grm
{	

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
	
	BoundingBox MergeBoundingBoxes(const BoundingBox& bb1,
								   const BoundingBox& bb2)
	{
		long unsigned int min_ux, min_uy, max_xw, max_yh;
		BoundingBox bb;

		min_ux = std::min(bb1.m_UX, bb2.m_UX);
		min_uy = std::min(bb1.m_UY, bb2.m_UY);
		max_xw = std::max(bb1.m_UX + bb1.m_W, bb2.m_UX + bb2.m_W);
		max_yh = std::max(bb1.m_UY + bb1.m_H, bb2.m_UY + bb2.m_H);

		bb.m_UX = min_ux;
		bb.m_UY = min_uy;
		bb.m_W = max_xw - min_ux;
		bb.m_H = max_yh - min_uy;

		return bb;
	}
	
} // end of namespace grm

#endif
