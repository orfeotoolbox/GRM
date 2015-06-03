#ifndef __LSRM_GRAPH_OPERATIONS_TXX
#define __LSRM_GRAPH_OPERATIONS_TXX
#include <otbImageFileReader.h>

namespace lsrm
{	
	template<class TSegmenter>
	void GraphOperations<TSegmenter>::InitNodes(ImageType * inputImg,
												GraphType& graph,
												SegmenterType& seg,
												CONNECTIVITY mask)
	{
		unsigned int width, height;
		
		{
			width = inputImg->GetLargestPossibleRegion().GetSize()[0];
			height = inputImg->GetLargestPossibleRegion().GetSize()[1];
		}
		
		const long unsigned int num_nodes = width * height;

		graph.m_Nodes.reserve(num_nodes);

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
			n->m_Pixels.push_back(i);
			graph.m_Nodes.push_back(n);
		}

		if(mask == FOUR)
		{
			for(auto& r : graph.m_Nodes)
			{
				long int neighborhood[4];
				FOURNeighborhood(neighborhood, r->m_Id, width, height);
				for(short j = 0; j < 4; ++j)
				{
					if(neighborhood[j] > -1)
						r->m_Edges.push_back(EdgeType( graph.m_Nodes[neighborhood[j]], 0, 1));
				}
			}
		}
		else
		{
			for(auto& r : graph.m_Nodes)
			{
				long int neighborhood[8];
				EIGHTNeighborhood(neighborhood, r->m_Id, width, height);
				for(short j = 0; j < 8; ++j)
				{
					if(neighborhood[j] > -1)
					{
						if(j % 2 > 0)
							r->m_Edges.push_back(EdgeType( graph.m_Nodes[neighborhood[j]], 0, 0));
						else
							r->m_Edges.push_back(EdgeType( graph.m_Nodes[neighborhood[j]], 0, 1));
					}
				}
			}
		}
		seg.InitFromImage();
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::UpdateMergingCosts(GraphType& graph,
														 SegmenterType& seg)
	{		
		float min_cost;
		long unsigned int min_id;
		std::size_t idx, min_idx;

		for(auto& r : graph.m_Nodes)
		{
			for(auto& edge : r->m_Edges)
				edge.m_CostUpdated = false;
		}

		for(auto& r : graph.m_Nodes)
		{
			min_cost = std::numeric_limits<float>::max();
			min_id = 0;
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
		for(auto& r : graph.m_Nodes)
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
														  const unsigned int width,
														  const unsigned int height)
	{
		/* Step 1: update the bounding box */
		a->m_Bbox = MergeBoundingBoxes(a->m_Bbox, b->m_Bbox);

		/* Update the list of pixels */
		auto begin = std::make_move_iterator(b->m_Pixels.begin());
		auto end = std::make_move_iterator(b->m_Pixels.end());
		a->m_Pixels.insert(a->m_Pixels.end(), begin, end);

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
	GraphOperations<TSegmenter>::PerfomOneIterationWithLMBF(GraphType& graph,
															SegmenterType& seg,
															const float threshold,
															const unsigned int width,
															const unsigned int height)
	{
		bool merged = false;

		/* Update the costs of merging between adjacent nodes */
		UpdateMergingCosts(graph, seg);

		for(auto& region : graph.m_Nodes)
		{
			
			auto res_node = CheckLMBF(region, threshold);

			if(res_node)
				{
					seg.UpdateSpecificAttributes(res_node, res_node->m_Edges.front().GetRegion());
					UpdateInternalAttributes(res_node, res_node->m_Edges.front().GetRegion(),
											 width, height);
					merged = true;
				}
		}

		RemoveExpiredNodes(graph);

		if(graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomOneIterationWithBF(GraphType& graph,
														  SegmenterType& seg,
														  const float threshold,
														  const unsigned int width,
														  const unsigned int height)
	{
		bool merged = false;

		/* Update the costs of merging between adjacent nodes */
		UpdateMergingCosts(graph, seg);

		for(auto& region : graph.m_Nodes)
		{
			NodePointerType res_node = CheckBF(region, threshold);

			if(res_node)
				{
					seg.UpdateSpecificAttributes(res_node, res_node->m_Edges.front().GetRegion());
					UpdateInternalAttributes(res_node, res_node->m_Edges.front().GetRegion(),
											 width, height);
					merged = true;
				}
		}

		RemoveExpiredNodes(graph);

		if(graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomAllIterationsWithLMBFAndConstThreshold(GraphType& graph,
																			  SegmenterType& seg,
																			  const float threshold,
																			  const unsigned int numberOfIterations,
																			  const unsigned int width,
																			  const unsigned int height)
	{
		bool merged = true;
		unsigned int iterations = 0;

		while(merged &&
			  iterations < numberOfIterations &&
			  graph.m_Nodes.size() > 1)
		{
			std::cout << "." << std::flush;
			++iterations;

			merged = PerfomOneIterationWithLMBF(graph, seg, threshold,
												width, height);
		}
		std::cout << std::endl;
		
		if(graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}

	template<class TSegmenter>
	bool
	GraphOperations<TSegmenter>::PerfomAllIterationsWithBFAndConstThreshold(GraphType& graph,
																			SegmenterType& seg,
																			const float threshold,
																			const unsigned int numberOfIterations,
																			const unsigned int width,
																			const unsigned int height)
	{
		bool merged = true;
		unsigned int maxNumberOfIterations;
		if(numberOfIterations < 1)
			maxNumberOfIterations = 75;
		else
			maxNumberOfIterations = numberOfIterations;
		
		unsigned int iterations = 0;

		while(merged &&
			  iterations < maxNumberOfIterations &&
			  graph.m_Nodes.size() > 1)
		{
			std::cout << "." << std::flush;
			++iterations;

			merged = PerfomOneIterationWithBF(graph, seg, threshold,
											  width, height);
		}
		std::cout << std::endl;

		if(graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}
	

} // end of namespace lsrm

#endif








