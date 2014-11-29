/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "grm-region.h"

namespace grm
{
	unsigned int
	Region::GetBboxIndex(unsigned int idx)
	{
		assert(idx >= 0 && idx < 2);
		return m_Bbox[idx];
	}

	unsigned int
	Region::GetBboxSize(unsigned int idx)
	{
		assert(idx >= 0 && idx < 2);
		return m_Bbox[2+idx];
	}

	void
	Region::SetBboxIndex(unsigned int idx, unsigned int v)
	{
		assert(idx >= 0 && idx < 2);
		m_Bbox[idx] = v;
	}

	void
	Region::SetBboxSize(unsigned int idx, unsigned int v)
	{
		assert(idx >= 0 && idx < 2);
		m_Bbox[idx+2] = v;
	}

	void
	Region::AddContour(short move)
	{
		assert(move >=0 && move < 4);
		m_Contour.push_back(move);
	}

	std::array<unsigned int, 4>
	Region::GetResultingBbox(std::shared_ptr<Region> r)
	{
		unsigned int min_row[3], max_row[3], min_col[3], max_col[3];

		min_row[0] = m_Bbox[1];
		max_row[0] = m_Bbox[1] + m_Bbox[3];
		min_col[0] = m_Bbox[0];
		max_col[0] = m_Bbox[0] + m_Bbox[2];

		min_row[1] = r->m_Bbox[1];
		max_row[1] = r->m_Bbox[1] + r->m_Bbox[3];
		min_col[1] = r->m_Bbox[0];
		max_col[1] = r->m_Bbox[0] + r->m_Bbox[2];

		if (min_row[0]<min_row[1]) min_row[2]=min_row[0]; else min_row[2]=min_row[1];
		if (min_col[0]<min_col[1]) min_col[2]=min_col[0]; else min_col[2]=min_col[1];
		if (max_row[0]>max_row[1]) max_row[2]=max_row[0]; else max_row[2]=max_row[1];
		if (max_col[0]>max_col[1]) max_col[2]=max_col[0]; else max_col[2]=max_col[1];

		std::array<unsigned int,4> res;
		res[1] = min_row[2];
		res[0] = min_col[2];
		res[3] = max_row[2] - min_row[2];
		res[2] = max_col[2] - min_col[2];
		return res;
	}

	void 
	Region::UpdateBbox(std::shared_ptr<Region> r)
	{
		m_Bbox = GetResultingBbox(r);
	}

	template<class DerivedRegion>
	void
	RegionBase<DerivedRegion>::AddNeighbor(std::shared_ptr<DerivedRegion> r,
											double cost,
											unsigned int len_bound)
	{
		assert(len_bound > 0);
		m_Neighbors.push_back(std::make_tuple(r, cost, len_bound));
	}

	template<class DerivedRegion>
	typename RegionBase<DerivedRegion>::EdgeIteratorType 
	RegionBase<DerivedRegion>::EdgeListBegin()
	{
		return m_Neighbors.begin();
	}

	template<class DerivedRegion>
	const typename RegionBase<DerivedRegion>::EdgeType& 
	RegionBase<DerivedRegion>::FrontEdgeList()
	{
		assert(m_Neighbors.size() >= 1);
		return m_Neighbors.front();
	}

	template<class DerivedRegion>
	typename RegionBase<DerivedRegion>::EdgeIteratorType 
	RegionBase<DerivedRegion>::EdgeListEnd()
	{
		return m_Neighbors.end();
	}

	template<class DerivedRegion>
	typename RegionBase<DerivedRegion>::EdgeIteratorType 
	RegionBase<DerivedRegion>::Erase(EdgeIteratorType eit)
	{
		return m_Neighbors.erase(eit);
	}

	template<class DerivedRegion>
	void 
	RegionBase<DerivedRegion>::SwapEdges(int idx1, int idx2)
	{
		assert(idx1 == 0);
		assert(idx2 > idx1 && idx2 < m_Neighbors.size());
		std::swap(m_Neighbors[idx1], m_Neighbors[idx2]);
	}

	template<class DerivedRegion>
	typename RegionBase<DerivedRegion>::EdgeIteratorType 
	RegionBase<DerivedRegion>::FindEdge(std::shared_ptr<DerivedRegion> r)
	{
		assert(r != nullptr);
		return std::find_if(m_Neighbors.begin(), m_Neighbors.end(), 
					[&](const EdgeType& n)->bool{
						return std::get<0>(n) == r;
				});
	}

	template<class DerivedRegion>
	unsigned int 
	RegionBase<DerivedRegion>::GetConnections(std::shared_ptr<DerivedRegion> r)
	{
		assert(r != nullptr);
		auto toR = FindEdge(r);
		assert(toR != EdgeListEnd());
		return std::get<2>(*toR);
	}

	template<class DerivedRegion>
	std::shared_ptr<DerivedRegion> 
	RegionBase<DerivedRegion>::GetClosestNeighbor()
	{
		assert(m_Neighbors.size() > 1);
		return std::get<0>(FrontEdgeList());
	}
} // end of namespace grm
