/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __GRM_REGION_H
#define __GRM_REGION_H

#include "macro-generator.h"

#include <cassert>
#include <memory>
#include <array>
#include <vector>
#include <tuple>
#include <bitset>
#include <utility>
#include <algorithm>

namespace grm
{
	class Region
	{
		public:

			Region(){}
			virtual ~Region(){}

			// Get / Set methods
			GetMacro(bool, Valid);
			GetMacro(bool, Expired);
			GetMacro(long unsigned int, Id);
			GetRefMacro(std::vector<std::bitset<2>>, Contour);
			std::array<unsigned int,4>& GetBbox(){return m_Bbox;}
			unsigned int GetBboxIndex(unsigned int);
			unsigned int GetBboxSize(unsigned int);

			SetMacro(bool, Valid);
			SetMacro(bool, Expired);
			SetMacro(long unsigned int, Id);
			SetMacro(std::vector<std::bitset<2>>, Contour);
			void SetBboxIndex(unsigned int, unsigned int);
			void SetBboxSize(unsigned int, unsigned int);

			void AddContour(short);
			std::array<unsigned int, 4> GetResultingBbox(std::shared_ptr<Region>);
			void UpdateBbox(std::shared_ptr<Region>);
			void UpdateContour(std::shared_ptr<Region>);

		protected:

			bool m_Valid; // True if the region has to be considered as the closest neighbor candidate
			bool m_Expired; // True if this region has to be removed from the list
			long unsigned int m_Id; // Identification of the region (represents also the coordinate of the start pixel)
			std::array<unsigned int, 4> m_Bbox;	// Rectangular bounding box of the region:
												// Idx 0 : upper left x coordinate
												// Idx 1 : upper left y coordinate
												// Idx 2 : size along x axis (width)
												// Idx 3 : size along y axis (height)
			std::vector<std::bitset<2>> m_Contour; // Freeman representation of the contour of a region
	};

	template<class DerivedRegion>
	class RegionBase : public Region
	{
		public:

			typedef std::tuple<
				std::shared_ptr<DerivedRegion>, // Pointer to the neighboring region
				double, // merging cost
				unsigned int // Length of the common boundary with this neighboring region
			> EdgeType;
			typedef std::vector<EdgeType> EdgeListType;
			typedef typename EdgeListType::iterator EdgeIteratorType;

			RegionBase(){}
			virtual ~RegionBase(){}

			EdgeIteratorType EdgeListBegin();
			EdgeIteratorType EdgeListEnd();
			EdgeIteratorType FindEdge(std::shared_ptr<DerivedRegion>);
			EdgeIteratorType Erase(EdgeIteratorType);
			const EdgeType& FrontEdgeList();
			std::shared_ptr<DerivedRegion> GetClosestNeighbor();
			void AddNeighbor(std::shared_ptr<DerivedRegion>, double, unsigned int);
			void SwapEdges(int idx1, int idx2);
			void UpdateNeighborhood(std::shared_ptr<DerivedRegion>);
			unsigned int GetConnections(std::shared_ptr<DerivedRegion>);

		protected:
			EdgeListType m_Neighbors;
	};
} // end of namespace grm

#include "grm-region.txx"
#endif
