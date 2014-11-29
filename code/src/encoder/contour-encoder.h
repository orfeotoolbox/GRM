/*=========================================================================

  Program: Shape Encoder library
  Language:  C++
  author:    Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __SHPENC_ShapeEncoder_H
#define __SHPENC_ShapeEncoder_H

#include "macro-generator.h"
#include "mask-neighborhood.h"
#include <vector>
#include <bitset>
#include <algorithm>

namespace shpenc
{
	class ShapeEncoder
	{
		public:

			typedef std::bitset<2>             Move;
			typedef typename std::vector<Move> Contour;
			typedef std::array<unsigned int, 4> BoundingBox;

			ShapeEncoder(){}
			~ShapeEncoder(){}

			SetMacro(unsigned int, Rows);
			SetMacro(unsigned int, Cols);
			GetMacro(unsigned int, Rows);
			GetMacro(unsigned int, Cols);

			Contour EncodeContour(long unsigned int, Contour&,
								long unsigned int, Contour&,
								BoundingBox&);
			std::vector<long unsigned int> GeneratePixels(long unsigned int, Contour&);
			//std::vector<long unsigned int> GenerateAllPixels(long unsigned int, Contour&, BoundingBox&);

		private:

			// Internal methods
			unsigned int ToBBoxId(long unsigned int, BoundingBox&);
			long unsigned int ToImgId(unsigned int, BoundingBox&);
			void RemoveInternalPixels(std::vector<short>&,
									  unsigned int, unsigned int);
			Contour EncodeNewContour(long unsigned int,
									BoundingBox&,
									std::vector<short>&);
			void UpdateSight(std::array<short, 4>&, short);
			void EncodeContourForTopPixel(unsigned int&,
										  short&,
										  short&,
									      Contour&,
										  const unsigned int);
			void EncodeContourForRightPixel(unsigned int&,
										    short&,
										    short&,
									        Contour&,
										    const unsigned int);
			void EncodeContourForBottomPixel(unsigned int&,
										     short&,
										     short&,
									         Contour&,
										     const unsigned int);
			void EncodeContourForLeftPixel(unsigned int&,
										   short&,
										   short&,
									       Contour&,
										   const unsigned int);

			bool IsInternalPixel(unsigned int cidx, std::vector<short>& bbox_array, 
								unsigned int bbox_size_x, unsigned int bbox_size_y);
	
			unsigned int m_Rows;
			unsigned int m_Cols;
	};
}

#endif
