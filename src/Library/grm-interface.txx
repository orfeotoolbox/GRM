/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "grm-interface.h"

namespace grm
{

	template<class TRegion>
	void 
	Interface<TRegion>::SetDimensionForShapeEncoder(unsigned int rows, unsigned int cols)
	{
		assert(rows > 0 && cols > 0);
		m_ShpEncoder.SetRows(rows);
		m_ShpEncoder.SetCols(cols);
	}

	template<class TRegion>
	std::pair<unsigned int, unsigned int> 
	Interface<TRegion>::GetDimensionOfInputImage()
	{
		return std::pair<unsigned int, unsigned int>(m_ShpEncoder.GetCols(), m_ShpEncoder.GetRows());
	}

	template<class TRegion>
	std::vector<long unsigned int> 
	Interface<TRegion>::GenerateFrontierPixels(RegionPointerType r)
	{
		return m_ShpEncoder.GeneratePixels(r->GetId(), r->GetContour());
	}

	template<class TRegion>
	std::vector<long unsigned int> 
	Interface<TRegion>::GenerateAllPixels(RegionPointerType r)
	{
		return m_ShpEncoder.GenerateAllPixels(r->GetId(), r->GetContour(), r->GetBbox());
	}

	template<class TRegion>
	void
	Interface<TRegion>::InitRegions(RegionListType& graph)
	{
		assert(graph.size() > 0);
		long unsigned int id_ = 0;
		std::for_each(graph.begin(), graph.end(), [&](RegionPointerType r){
			r->SetId(id_);
			r->SetBboxIndex(0, id_ % m_ShpEncoder.GetCols());
			r->SetBboxIndex(1, id_ / m_ShpEncoder.GetCols());
			r->SetBboxSize(0,1);
			r->SetBboxSize(1,1);
			r->SetValid(true);
			r->SetExpired(false);
			r->AddContour(1); r->AddContour(2);
			r->AddContour(3); r->AddContour(0);

			auto neighbors = mask::CrossNeighborhood(r->GetId(), m_ShpEncoder.GetCols(), 
													m_ShpEncoder.GetRows());
			for(short j = 0; j < 4; ++j)
			{
				if(neighbors[j] > -1)
					r->AddNeighbor(graph[neighbors[j]], 0, 1);
			}
			++id_;
		});
	}

	template<class TRegion>
	void
	Interface<TRegion>::UpdateMergingCosts(RegionListType& graph,
											MergingCostFunction& func)
	{
		double min_cost;
		long unsigned int min_id;
		int idx_tpl, idx_min_tpl;

		std::for_each(graph.begin(), graph.end(), [&](RegionPointerType r){

			r->SetValid(true);
			r->SetExpired(false);

			min_cost = std::numeric_limits<double>::max();
			idx_tpl = 0;
			idx_min_tpl = 0;
			std::for_each(r->EdgeListBegin(), r->EdgeListEnd(), [&](EdgeType& e){
				std::get<1>(e) = func(r, std::get<0>(e));
				if(min_cost > std::get<1>(e))
				{
					min_cost = std::get<1>(e);
					min_id = std::get<0>(e)->GetId();
					idx_min_tpl = idx_tpl;
					++idx_tpl;
				}
				else if(min_cost == std::get<1>(e) && min_id > std::get<0>(e)->GetId())
				{
					min_id = std::get<0>(e)->GetId();
					idx_min_tpl = idx_tpl;
					++idx_tpl;
				}
				else
					++idx_tpl;
			});

			if(idx_min_tpl > 0)
				r->SwapEdges(0, idx_min_tpl);
		});
	}

	template<class TRegion>
	typename Interface<TRegion>::RegionPointerType
	Interface<TRegion>::CheckLMBF(RegionPointerType r, double thresh)
	{
		assert(r != nullptr);
		if(r->GetValid())
		{
			auto cost = std::get<1>(r->FrontEdgeList());
			if(cost < thresh)
			{
				auto closest_neigh = std::get<0>(r->FrontEdgeList());
				if(closest_neigh->GetValid())
				{
					if(std::get<0>(closest_neigh->FrontEdgeList()) == r)
					{
						if(r->GetId() < closest_neigh->GetId())
							return r;
						else
							return closest_neigh;
					}
					else
						return nullptr;
				}
				else
					return nullptr;
			}
			else
				return nullptr;
		}
		else
			return nullptr;
	}

	template<class TRegion>
	typename Interface<TRegion>::RegionPointerType
	Interface<TRegion>::CheckBF(RegionPointerType r, double thresh)
	{
		assert(r != nullptr);
		if(r->GetValid())
		{
			auto cost = std::get<1>(r->FrontEdgeList());
			if(cost < thresh)
			{
				auto closest_neigh = std::get<0>(r->FrontEdgeList());
				if(closest_neigh->GetValid())
				{
					if(r->GetId() < closest_neigh->GetId())
						return r;
					else
						return closest_neigh;
				}
				else
					return nullptr;
			}
			else
				return nullptr;
		}
		else
			return nullptr;
	}

	template<class TRegion>
	void 
	Interface<TRegion>::UpdateContour(RegionPointerType r1, RegionPointerType r2)
	{
		r1->UpdateBbox(r2);
		r1->SetContour(m_ShpEncoder.EncodeContour(r1->GetId(), r1->GetContour(), 
													r2->GetId(), r2->GetContour(),
													r1->GetBbox()));
	}

	template<class TRegion>
	void
	Interface<TRegion>::UpdateNeighborhood(RegionPointerType r1, RegionPointerType r2)
	{
		unsigned int connections;
		for(auto adj_n = r2->EdgeListBegin(); adj_n != r2->EdgeListEnd(); ++adj_n)
		{
			auto adj_nr = std::get<0>(*adj_n);
			auto toR2 = adj_nr->FindEdge(r2);

			if(toR2 == adj_nr->EdgeListEnd())
				adj_nr->SetValid(false);

			connections = std::get<2>(*toR2);
			adj_nr->Erase(toR2);

			if(adj_nr != r1)
			{
				auto toR1 = adj_nr->FindEdge(r1);

				if(toR1 == adj_nr->EdgeListEnd())
				{
					adj_nr->AddNeighbor(r1, 0, connections);
					r1->AddNeighbor(adj_nr, 0, connections);
				}
				else
				{
					// We increment the connections between adj_nr and r1 by connections
					std::get<2>(*toR1) += connections;
					auto toAdjNr = r1->FindEdge(adj_nr);
					std::get<2>(*toAdjNr) += connections;
				}
			}
		}
	}

	template<class TRegion>
	void
	Interface<TRegion>::Update(RegionPointerType r1, RegionPointerType r2)
	{
		assert(r1 != nullptr && r2 != nullptr);
		UpdateContour(r1,r2);
		UpdateNeighborhood(r1, r2);
		r1->SetValid(false);
		r2->SetValid(false);
		r2->SetExpired(true);
	}

	template<class TRegion>
	void
	Interface<TRegion>::RemoveExpiredVertices(RegionListType& graph)
	{
		auto exp = std::remove_if(graph.begin(), graph.end(), [](RegionPointerType r)->bool{
			return r->GetExpired();
		});
		graph.erase(exp, graph.end());
	}
}
