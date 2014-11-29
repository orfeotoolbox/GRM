/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __GRM_INTERFACE_H
#define __GRM_INTERFACE_H

#include "contour-encoder.h"

#include <memory>
#include <tuple>
#include <functional>
#include <limits>

namespace grm
{
	template<class TRegion>
	class Interface
	{
		public:

			typedef std::shared_ptr<TRegion> RegionPointerType;
			typedef std::tuple<RegionPointerType, double, unsigned int> EdgeType;
			typedef std::vector<RegionPointerType> RegionListType;
			typedef std::function<double(RegionPointerType, RegionPointerType)> MergingCostFunction;

			void InitRegions(RegionListType&);
			void UpdateMergingCosts(RegionListType&, MergingCostFunction&);
			RegionPointerType CheckLMBF(RegionPointerType, double);
			void UpdateContour(RegionPointerType, RegionPointerType);
			void UpdateNeighborhood(RegionPointerType, RegionPointerType);
			void Update(RegionPointerType r1, RegionPointerType r2);
			void RemoveExpiredVertices(RegionListType& graph);

			std::vector<long unsigned int> GenerateFrontierPixels(RegionPointerType);
			//std::vector<long unsigned int> GenerateAllPixels(RegionPointerType);

			// Get / Set methods
			std::pair<unsigned int, unsigned int> GetDimensionOfInputImage();
			void SetDimensionForShapeEncoder(unsigned int, unsigned int);

		private:
			shpenc::ShapeEncoder m_ShpEncoder;
	};
} // end of namespace grm

#include "grm-interface.txx"
#endif
