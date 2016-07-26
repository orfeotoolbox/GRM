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
#ifndef GRM_GRAPH_OPERATIONS_TXX
#define GRM_GRAPH_OPERATIONS_TXX
#include <otbImageFileReader.h>

namespace grm
{	
	template<class TSegmenter>
	void GraphOperations<TSegmenter>::InitNodes(ImageType * inputImg,
												SegmenterType& seg,
												CONNECTIVITY mask)
	{
		unsigned int width, height;
		
		{
			width = inputImg->GetLargestPossibleRegion().GetSize()[0];
			height = inputImg->GetLargestPossibleRegion().GetSize()[1];
		}
		
		const long unsigned int num_nodes = width * height;

		seg.m_Graph.m_Nodes.reserve(num_nodes);

		for(long unsigned int i = 0;
			i < num_nodes;
			++i)
		{
			NodePointerType n(new NodeType);
			n->m_Id = i;
			n->m_Valid = true;
			n->m_Expired = false;
			n->m_IsMerged = true; // force to compute costs for the first iteration
			n->m_Perimeter = 4;
			n->m_Area = 1;
			n->m_Bbox.m_UX = i % width;
			n->m_Bbox.m_UY = i / width;
			n->m_Bbox.m_W = 1;
			n->m_Bbox.m_H = 1;

			// An initial contour is the one aroun a pixel
			ContourOperator::Push1(n->m_Contour);
			ContourOperator::Push2(n->m_Contour);
			ContourOperator::Push3(n->m_Contour);
			ContourOperator::Push0(n->m_Contour);
			
			seg.m_Graph.m_Nodes.push_back(n);
		}

		if(mask == FOUR)
		{
			for(auto& r : seg.m_Graph.m_Nodes)
			{
				long int neighborhood[4];
				FOURNeighborhood(neighborhood, r->m_Id, width, height);
				for(short j = 0; j < 4; ++j)
				{
					if(neighborhood[j] > -1)
						r->m_Edges.push_back(EdgeType( seg.m_Graph.m_Nodes[neighborhood[j]], 0, 1));
				}
			}
		}
		else
		{
			for(auto& r : seg.m_Graph.m_Nodes)
			{
				long int neighborhood[8];
				EIGHTNeighborhood(neighborhood, r->m_Id, width, height);
				for(short j = 0; j < 8; ++j)
				{
					if(neighborhood[j] > -1)
					{
						if(j % 2 > 0)
							r->m_Edges.push_back(EdgeType( seg.m_Graph.m_Nodes[neighborhood[j]], 0, 0));
						else
							r->m_Edges.push_back(EdgeType( seg.m_Graph.m_Nodes[neighborhood[j]], 0, 1));
					}
				}
			}
		}
		seg.InitFromImage();
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::UpdateMergingCosts(SegmenterType& seg)
	{		
		float min_cost;
		long unsigned int min_id  = 0;
		std::size_t idx, min_idx;

		for(auto& r : seg.m_Graph.m_Nodes)
		{
			for(auto& edge : r->m_Edges)
				edge.m_CostUpdated = false;
		}

		for(auto& r : seg.m_Graph.m_Nodes)
		{
			min_cost = std::numeric_limits<float>::max();
			idx = 0;
			min_idx = 0;

			r->m_Expired = false;
			r->m_Valid = true;

			for(auto& edge : r->m_Edges)
			{
				auto neighborR = edge.GetRegion();

				// Compute the cost if necessary
				if(!edge.m_CostUpdated && (neighborR->m_IsMerged || r->m_IsMerged))
				{
					auto edgeFromNeighborToR = FindEdge(neighborR, r);
					edge.m_Cost = seg.ComputeMergingCost(r, neighborR);
					edgeFromNeighborToR->m_Cost = edge.m_Cost;
					edge.m_CostUpdated = true;
					edgeFromNeighborToR->m_CostUpdated = true;
				}

				// Check if the cost of the edge is the minimum
				if(min_cost > edge.m_Cost)
				{
					min_cost = edge.m_Cost;
					min_id = neighborR->m_Id;
					min_idx = idx;
				}
				else if(min_cost == edge.m_Cost)
				{
					if(min_id > neighborR->m_Id)
					{
						min_id = neighborR->m_Id;
						min_idx = idx;
					}
				}
				++idx;	
			}

			assert(min_idx < r->m_Edges.size());
			std::swap(r->m_Edges[0], r->m_Edges[min_idx]);
				
		}

		// Reset the merge flag for all the regions.
		for(auto& r : seg.m_Graph.m_Nodes)
			r->m_IsMerged = false;
	}

	template<class TSegmenter>
	typename GraphOperations<TSegmenter>::NodePointerType
	GraphOperations<TSegmenter>::CheckLMBF(NodePointerType a, float t)
	{
		if(a->m_Valid)
		{
			float cost = a->m_Edges.front().m_Cost;
			
			if(cost < t)
			{
				NodePointerType b = a->m_Edges.front().GetRegion();

				if( b->m_Valid)
				{
					NodePointerType best_b = b->m_Edges.front().GetRegion();

					if(a == best_b)
					{
						if(a->m_Id < b->m_Id)
							return a;
						else
							return b;
					}
					else
						return NodePointerType();
				}
				else
					return NodePointerType();
			}
			else
				return NodePointerType();
		}
		else
			return NodePointerType();
	}

	template<class TSegmenter>
	typename GraphOperations<TSegmenter>::NodePointerType
	GraphOperations<TSegmenter>::CheckBF(NodePointerType a, float t)
	{
		if(a->m_Valid)
		{
			float cost = a->m_Edges.front().m_Cost;

			if( cost < t )
			{
				NodePointerType b = a->m_Edges.front().GetRegion();

				if(b->m_Valid)
				{
					if( a->m_Id < b->m_Id )
						return a;
					else
						return b;
				}
				else
					return NodePointerType();
			}
			else
				return NodePointerType();
		}
		else
			return NodePointerType();
	}

	template<class TSegmenter>
	typename GraphOperations<TSegmenter>::EdgeIterator
	GraphOperations<TSegmenter>::FindEdge(NodePointerType n, NodePointerType target)
	{
		return std::find_if(n->m_Edges.begin(), n->m_Edges.end(),[&](EdgeType& e)->bool{
				return e.GetRegion() == target;
			});
	}

	template<class TSegmenter>
	void
	GraphOperations<TSegmenter>::UpdateNeighbors(NodePointerType a, NodePointerType b)
	{
		unsigned int boundary;

		/* Explore the neighbors of b */
		for (auto& edge : b->m_Edges)
		{
			// Retrieve the edge targeting node b.
			auto neigh_b = edge.GetRegion();
			auto toB = FindEdge(neigh_b, b);

			/* If the edge tageting to node b is the first then
			   the corresponding node is not valid anymore. */
			if(toB == neigh_b->m_Edges.begin())
				neigh_b->m_Valid = false;

			/* Keep in memory the boundary between node b
			   and node neigh_b */
			boundary = edge.m_Boundary;

			/*
			  We can remove safely the edge from node neigh_b
			  targeting to node b
			*/
			neigh_b->m_Edges.erase(toB);

			if(neigh_b != a)
			{
				/* Retrieve the edge targeting to node a. */
				auto toA = FindEdge(neigh_b, a);

				if( toA == neigh_b->m_Edges.end() )
				{
					/* No edge exists between node a and node neigh_b. */

					/* Add an edge from node neigh_b targeting node a. */
					neigh_b->m_Edges.push_back(EdgeType(a, 0, boundary));

					/* Add an edge from node a targeting node neigh_b. */
					a->m_Edges.push_back(EdgeType(neigh_b, 0, boundary));
				}
				else
				{
					/* An edge exists between node a and node neigh_b. */

					/* Increment the boundary of the edge from node neigh_b
					   targeting to node a. */
					toA->m_Boundary += boundary;

					/* Increment the boundary of the edge from node a
					   targeting to node neigh_b. */
					auto toNeighB = FindEdge(a, neigh_b);
					toNeighB->m_Boundary += boundary;
				}
			}
			
		}
		
	}

	template<class TSegmenter>
	void
	GraphOperations<TSegmenter>::UpdateInternalAttributes(NodePointerType a,
														  NodePointerType b,
														  const unsigned int width)
	{

		lp::BoundingBox mergedBBox;
		lp::Contour mergedContour;		
		ContourOperator::MergeContour(mergedContour, mergedBBox, a->m_Contour,
									  b->m_Contour, a->m_Bbox, b->m_Bbox,
									  a->m_Id, b->m_Id, width);
		
		/* Step 1: update the bounding box */
		a->m_Bbox = mergedBBox;//MergeBoundingBoxes(a->m_Bbox, b->m_Bbox);

		/* Step 2: update the contour */
		a->m_Contour = mergedContour;

		/* Step 2 : update perimeter and area attributes */
		EdgeIterator toB = FindEdge(a, b);
		a->m_Perimeter += (b->m_Perimeter - 2 * toB->m_Boundary);
		a->m_Area += b->m_Area;
			
		/* Step 2: update the neighborhood */
		UpdateNeighbors(a,b);
		
		/* Step 3: update the node' states */
		a->m_Valid = false;
		b->m_Valid = false;
		b->m_Expired = true;
		a->m_IsMerged = true;
	}

	template<class TSegmenter>
	void
	GraphOperations<TSegmenter>::RemoveExpiredNodes(GraphType& graph)
	{
		NodeIterator nit = std::remove_if(graph.m_Nodes.begin(), graph.m_Nodes.end(), [](NodePointerType r)->bool{
				return r->m_Expired;
			});
		graph.m_Nodes.erase(nit, graph.m_Nodes.end());
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomOneIterationWithLMBF(SegmenterType& seg)
	{
		bool merged = false;

		/* Update the costs of merging between adjacent nodes */
		UpdateMergingCosts(seg);

		for(auto& region : seg.m_Graph.m_Nodes)
		{
			
			auto res_node = CheckLMBF(region, seg.GetThreshold());

			if(res_node)
				{
					seg.UpdateSpecificAttributes(res_node, res_node->m_Edges.front().GetRegion());
					UpdateInternalAttributes(res_node, res_node->m_Edges.front().GetRegion(),
											 seg.GetImageWidth());
					merged = true;
				}
		}

		RemoveExpiredNodes(seg.m_Graph);

		if(seg.m_Graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomAllIterationsWithLMBFAndConstThreshold(SegmenterType& seg)
	{
		bool merged = true;
		unsigned int maxNumberOfIterations;
		unsigned int iterations = 0;
		unsigned int numberOfIterations = seg.GetNumberOfIterations();

		if(numberOfIterations < 1)
			maxNumberOfIterations = 200;
		else
			maxNumberOfIterations = numberOfIterations;

		while(merged &&
			  iterations < maxNumberOfIterations &&
			  seg.m_Graph.m_Nodes.size() > 1)
		{
			std::cout << "." << std::flush;
			++iterations;

			merged = PerfomOneIterationWithLMBF(seg);
		}
		std::cout << std::endl;
		
		if(seg.m_Graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	/* New !!! utilisation of a dither matrix */

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomAllDitheredIterationsWithBF(SegmenterType& seg)
	{
		bool merged = true;
		unsigned int maxNumberOfIterations;
		unsigned int numberOfIterations = seg.GetNumberOfIterations();
		
		if(numberOfIterations < 1)
			maxNumberOfIterations = 200;
		else
			maxNumberOfIterations = numberOfIterations;

		unsigned int iterations = 0;

		while(merged &&
			  iterations < maxNumberOfIterations &&
			  seg.m_Graph.m_Nodes.size() > 1)
		{
			std::cout << "." << std::flush;
			++iterations;

			merged = PerfomOneDitheredIterationWithBF(seg);
		}
		std::cout << std::endl;

		if(seg.m_Graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomOneDitheredIterationWithBF(SegmenterType& seg)
	{
		bool merged = false;

		std::vector<long unsigned int> randomIndices(seg.m_Graph.m_Nodes.size());
		std::iota(randomIndices.begin(), randomIndices.end(), 0);
		std::shuffle(randomIndices.begin(), randomIndices.end(), std::mt19937{std::random_device{}()});

		for(const auto& i : randomIndices)
		{
			if(seg.m_Graph.m_Nodes[i]->m_Valid == true)
			{
				auto currSeg = seg.m_Graph.m_Nodes[i];
			
				// This segment is marked as used.
				currSeg->m_Valid = false;

				// Compute cost with all its neighbors
				ComputeMergingCostsUsingDither(currSeg, seg);

				// Get the most similar segment
				auto bestSeg = currSeg->m_Edges.front().GetRegion();

				if(currSeg->m_Edges.front().m_Cost < seg.GetThreshold() && !bestSeg->m_Expired)
				{
					merged = true;
				
					if(currSeg->m_Id < bestSeg->m_Id)
					{
						seg.UpdateSpecificAttributes(currSeg, bestSeg);
						UpdateInternalAttributes(currSeg, bestSeg, seg.GetImageWidth());

						for(auto& edge : currSeg->m_Edges)
						{
							edge.m_CostUpdated = false;
							auto toNeigh = FindEdge(edge.GetRegion(), currSeg);
							toNeigh->m_CostUpdated = false;
						}
					}
					else
					{
						seg.UpdateSpecificAttributes(bestSeg, currSeg);
						UpdateInternalAttributes(bestSeg, currSeg, seg.GetImageWidth());

						for(auto& edge : bestSeg->m_Edges)
						{
							edge.m_CostUpdated = false;
							auto toNeigh = FindEdge(edge.GetRegion(), bestSeg);
							toNeigh->m_CostUpdated = false;
						}
					}
				}
			}
		}

		RemoveExpiredNodes(seg.m_Graph);

		// Mark all the segments to be valid

		if(seg.m_Graph.m_Nodes.size() < 2)
			return false;

		for(auto& r : seg.m_Graph.m_Nodes)
			r->m_Valid = true;

		return merged;
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::ComputeMergingCostsUsingDither(NodePointerType r, SegmenterType& seg)
	{

		float min_cost = std::numeric_limits<float>::max();
		std::size_t idx = 0, min_idx = 0;


		for(auto& edge : r->m_Edges)
		{
			// Compute the cost if the neighbor is not expired and the cost has to be updated
			if(!edge.GetRegion()->m_Expired)
			{
				auto neighborR = edge.GetRegion();

				// Compute the cost if necessary
				if(!edge.m_CostUpdated)
				{
					auto edgeFromNeighborToR = FindEdge(neighborR, r);
					edge.m_Cost = seg.ComputeMergingCost(r, neighborR);
					edgeFromNeighborToR->m_Cost = edge.m_Cost;
					edge.m_CostUpdated = true;
					edgeFromNeighborToR->m_CostUpdated = true;
				}

				// Check if the cost of the edge is the minimum
				if(min_cost > edge.m_Cost)
				{
					min_cost = edge.m_Cost;
					min_idx = idx;
				}
				
				++idx;	
			}
		}
		assert(min_idx < r->m_Edges.size());
		std::swap(r->m_Edges[0], r->m_Edges[min_idx]);			
	}	
} // end of namespace grm

#endif