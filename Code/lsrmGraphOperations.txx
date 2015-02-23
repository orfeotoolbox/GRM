#ifndef __LSRM_GRAPH_OPERATIONS_TXX
#define __LSRM_GRAPH_OPERATIONS_TXX
#include <otbImageFileReader.h>

namespace lsrm
{
	template<class TSegmenter>
	void GraphOperations<TSegmenter>::InitNodes(GraphType& graph,
												SegmenterType& seg,
												const std::string& inputFileName,
												CONNECTIVITY mask)
	{
		unsigned int width, height;
		
		{
			typedef typename TSegmenter::ImageType ImageType;
			typedef otb::ImageFileReader<ImageType> ReaderType;
			typename ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(inputFileName);
			reader->UpdateOutputInformation();

			width = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
			height = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];
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
			n->m_Perimeter = 4;
			n->m_Area = 1;
			n->m_Bbox.m_UX = i % width;
			n->m_Bbox.m_UY = i / width;
			n->m_Bbox.m_W = 1;
			n->m_Bbox.m_H = 1;
			n->m_Contour.push_back(1);
			n->m_Contour.push_back(2);
			n->m_Contour.push_back(3);
			n->m_Contour.push_back(0);
			graph.m_Nodes.push_back(n);
		}

		if(mask == FOUR)
		{
			for(long unsigned int i = 0;
				i < num_nodes;
				++i)
			{
				long int neighborhood[4];
				FOURNeighborhood(neighborhood, i, width, height);
				for(short j = 0; j < 4; ++j)
				{
					if(neighborhood[j] > -1)
					{
						EdgeType edge;
						edge.m_Boundary = 1;
						edge.m_Cost = 0.0f;
						edge.m_Target = graph.m_Nodes[neighborhood[j]];
						graph.m_Nodes[i]->m_Edges.push_back(edge);
					}
				}
			}
		}
		else
		{
			for(long unsigned int i = 0;
				i < num_nodes;
				++i)
			{
				long int neighborhood[8];
				EIGHTNeighborhood(neighborhood, i, width, height);
				for(short j = 0; j < 8; ++j)
				{
					if(neighborhood[j] > -1)
					{
						EdgeType edge;
						if(j % 2 > 0)
							edge.m_Boundary = 0;
						else
							edge.m_Boundary = 1;
						edge.m_Cost = 0.0f;
						edge.m_Target = graph.m_Nodes[neighborhood[j]];
						graph.m_Nodes[i]->m_Edges.push_back(edge);
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
		
		for (NodeIterator it = graph.m_Nodes.begin();
			 it != graph.m_Nodes.end(); ++it)
		{
			// Reset boolean flags
			(*it)->m_Valid = true;
			(*it)->m_Expired = false;

			min_cost = std::numeric_limits<float>::max();
			idx = 0;
			min_idx = 0;
			
			for (EdgeIterator et = (*it)->m_Edges.begin();
				 et != (*it)->m_Edges.end(); ++et)
			{

				et->m_Cost = seg.ComputeMergingCost(*it, et->m_Target);

				if(min_cost > et->m_Cost)
				{
					min_cost = et->m_Cost;
					min_id = et->m_Target->m_Id;
					min_idx = idx;
					++idx;
				}
				else if(min_cost == et->m_Cost && min_id > et->m_Target->m_Id)
				{
					min_id = et->m_Target->m_Id;
					min_idx = idx;
					++idx;
				}
				else
					++idx;
			}

			if(min_idx > 0)
				std::swap((*it)->m_Edges[0], (*it)->m_Edges[min_idx]);
		}
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
				NodePointerType b = a->m_Edges.front().m_Target;

				if( b->m_Valid)
				{
					NodePointerType best_b = b->m_Edges.front().m_Target;

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
				NodePointerType b = a->m_Edges.front().m_Target;

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
		return std::find_if(n->m_Edges.begin(), n->m_Edges.end(),
							EdgeEqualTo(target));
	}

	template<class TSegmenter>
	void
	GraphOperations<TSegmenter>::UpdateNeighbors(NodePointerType a, NodePointerType b)
	{
		unsigned int boundary;

		/* Explore the neighbors of b */
		for (EdgeIterator eit = b->m_Edges.begin();
			 eit != b->m_Edges.end(); ++eit)
		{
			// Retrieve the edge targeting node b.
			NodePointerType neigh_b = eit->m_Target;
			EdgeIterator toB = FindEdge(neigh_b, b);

			/* If the edge tageting to node b is the first then
			   the corresponding node is not valid anymore. */
			if(toB == neigh_b->m_Edges.begin())
				neigh_b->m_Valid = false;

			/* Keep in memory the boundary between node b
			   and node neigh_b */
			boundary = eit->m_Boundary;

			/*
			  We can remove safely the edge from node neigh_b
			  targeting to node b
			*/
			neigh_b->m_Edges.erase(toB);

			if(neigh_b != a)
			{
				/* Retrieve the edge targeting to node a. */
				EdgeIterator toA = FindEdge(neigh_b, a);

				if( toA == neigh_b->m_Edges.end() )
				{
					/* No edge exists between node a and node neigh_b. */

					/* Add an edge from node neigh_b targeting node a. */
					EdgeType edge;
					edge.m_Boundary = boundary;
					edge.m_Cost = 0.0f;
					edge.m_Target = a;
					neigh_b->m_Edges.push_back(edge);

					/* Add an edge from node a targeting node neigh_b. */
					edge.m_Target = neigh_b;
					a->m_Edges.push_back(edge);
				}
				else
				{
					/* An edge exists between node a and node neigh_b. */

					/* Increment the boundary of the edge from node neigh_b
					   targeting to node a. */
					toA->m_Boundary += boundary;

					/* Increment the boundary of the edge from node a
					   targeting to node neigh_b. */
					EdgeIterator toNeighB = FindEdge(a, neigh_b);
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
		/* Step 1: update the contour */
		a->m_Bbox = ContourOperations::MergeBoundingBoxes(a->m_Bbox, b->m_Bbox);
		a->m_Contour = ContourOperations::MergeContours(a->m_Id, b->m_Id, a->m_Contour, b->m_Contour,
														a->m_Bbox, width, height);

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
	}

	template<class TSegmenter>
	void
	GraphOperations<TSegmenter>::RemoveExpiredNodes(GraphType& graph)
	{
		NodeIterator nit = std::remove_if(graph.m_Nodes.begin(), graph.m_Nodes.end(), IsExpired());
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

		for(NodeIterator nit = graph.m_Nodes.begin();
			nit != graph.m_Nodes.end(); ++nit)
		{
			
			NodePointerType res_node = CheckLMBF(*nit, threshold);

			if(res_node)
				{
					seg.UpdateSpecificAttributes(res_node, res_node->m_Edges.front().m_Target);
					UpdateInternalAttributes(res_node, res_node->m_Edges.front().m_Target,
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

		for(NodeIterator nit = graph.m_Nodes.begin();
			nit != graph.m_Nodes.end(); ++nit)
		{
			NodePointerType res_node = CheckBF(*nit, threshold);

			if(res_node)
				{
					seg.UpdateSpecificAttributes(res_node, res_node->m_Edges.front().m_Target);
					UpdateInternalAttributes(res_node, res_node->m_Edges.front().m_Target,
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
		unsigned int iterations = 0;

		while(merged &&
			  iterations < numberOfIterations &&
			  graph.m_Nodes.size() > 1)
		{
			++iterations;

			merged = PerfomOneIterationWithBF(graph, seg, threshold,
											  width, height);
		}

		if(graph.m_Nodes.size() < 2)
			return false;

		return merged;
	}
	
	template<class TSegmenter>
	void GraphOperations<TSegmenter>::LargeScaleInitNodes(GraphType& graph,
														  SegmenterType& seg,
														  const unsigned int tileWidth,
														  const unsigned int tileHeight,
														  const unsigned int nbTilesX,
														  const unsigned int nbTilesY,
														  const unsigned int tileId,
														  const unsigned int margin,
														  const unsigned int imageWidth,
														  const unsigned int imageHeight,
														  CONNECTIVITY mask)
	{
		const unsigned int tileX = tileId % nbTilesX;
		const unsigned int tileY = tileId / nbTilesX;

		unsigned int xsize = tileWidth, ysize = tileHeight;
		unsigned int xmargin = 0, ymargin = 0;
		
		if(tileX > 0)
		{
			xmargin = margin;
			xsize += margin;
		}
		if(tileY > 0)
		{
			ymargin = margin;
			ysize += margin;
		}
		
		if(tileX < nbTilesX - 1)
			xsize += margin;

		if(tileY < nbTilesY - 1)
			ysize += margin;
		
		const long unsigned int num_nodes = xsize * ysize;
		graph.m_Nodes.reserve(num_nodes);

		unsigned int xtile, ytile, ximg, yimg;

		for(long unsigned int i = 0;
			i < num_nodes;
			++i)
		{
			NodePointerType n(new NodeType);

			/* Compute the coordinates of its first pixel */
			xtile = i % xsize;
			ytile = i / xsize;
			ximg = tileX * tileWidth + xtile - xmargin;
			yimg = tileY * tileHeight + ytile - ymargin;
			n->m_Id = yimg * imageWidth + ximg;
			
			n->m_Valid = true;
			n->m_Expired = false;
			n->m_Perimeter = 4;
			n->m_Area = 1;
			n->m_Bbox.m_UX = ximg;
			n->m_Bbox.m_UY = yimg;
			n->m_Bbox.m_W = 1;
			n->m_Bbox.m_H = 1;
			n->m_Contour.push_back(1);
			n->m_Contour.push_back(2);
			n->m_Contour.push_back(3);
			n->m_Contour.push_back(0);
			graph.m_Nodes.push_back(n);
		}

		if(mask == FOUR)
		{
			for(long unsigned int i = 0;
				i < num_nodes;
				++i)
			{
				long int neighborhood[4];
				FOURNeighborhood(neighborhood, i, xsize, ysize);
				for(short j = 0; j < 4; ++j)
				{
					if(neighborhood[j] > -1)
					{
						EdgeType edge;
						edge.m_Boundary = 1;
						edge.m_Cost = 0.0f;
						edge.m_Target = graph.m_Nodes[neighborhood[j]];
						graph.m_Nodes[i]->m_Edges.push_back(edge);
					}
				}
			}
		}
		else
		{
			for(long unsigned int i = 0;
				i < num_nodes;
				++i)
			{
				long int neighborhood[8];
				EIGHTNeighborhood(neighborhood, i, xsize, ysize);
				for(short j = 0; j < 8; ++j)
				{
					if(neighborhood[j] > -1)
					{
						EdgeType edge;
						if(j % 2 > 0)
							edge.m_Boundary = 0;
						else
							edge.m_Boundary = 1;
						edge.m_Cost = 0.0f;
						edge.m_Target = graph.m_Nodes[neighborhood[j]];
						graph.m_Nodes[i]->m_Edges.push_back(edge);
					}
				}
			}
		}
		seg.InitFromImage();
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::RemoveEdgesToUnstableRegion(NodePointerType r)
	{
		for (EdgeIterator et = r->m_Edges.begin();
			 et != r->m_Edges.end(); ++et)
		{
			EdgeIterator toR = FindEdge(et->m_Target, r);
			et->m_Target->m_Edges.erase(toR);
		}
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::RemoveUnstableRegions(GraphType& graph,
															unsigned int lowerRow,
															unsigned int upperRow,
															unsigned int lowerCol,
															unsigned int upperCol,
															unsigned int imageWidth)
	{
		for(NodeIterator nit = graph.m_Nodes.begin();
			nit != graph.m_Nodes.end(); ++nit)
		{
			if((*nit)->m_Bbox.m_UX >= lowerCol &&
			   (*nit)->m_Bbox.m_UY >= lowerRow &&
			   ((*nit)->m_Bbox.m_W + (*nit)->m_Bbox.m_UX - 1) <= upperCol &&
			   ((*nit)->m_Bbox.m_H + (*nit)->m_Bbox.m_UY - 1) <= upperRow)
				continue;
			else if((*nit)->m_Bbox.m_UX > upperCol ||
					(*nit)->m_Bbox.m_UY > upperRow ||
					((*nit)->m_Bbox.m_W + (*nit)->m_Bbox.m_UX - 1) < lowerCol ||
					((*nit)->m_Bbox.m_H + (*nit)->m_Bbox.m_UY - 1) < lowerRow)
			{
				RemoveEdgesToUnstableRegion(*nit);
				(*nit)->m_Expired = true;
			}
			else
			{
				bool unstable = true;
				unsigned int pix_col, pix_row;
				ContourOperations::PixelList pixels = ContourOperations::GeneratePixels((*nit)->m_Id,
																						(*nit)->m_Contour,
																						(*nit)->m_Bbox,
																						imageWidth);
				for (ContourOperations::PixelConstIterator pit = pixels.begin();
					 pit != pixels.end(); ++pit)
				{
					pix_col = (*pit) % imageWidth;
					pix_row = (*pit) / imageWidth;

					if(pix_col >= lowerCol && pix_col <= upperCol &&
					   pix_row >= lowerRow && pix_row <= upperRow)
					{
						unstable = false;
						break;
					}
				}

				if(unstable)
				{
					RemoveEdgesToUnstableRegion(*nit);
					(*nit)->m_Expired = true;
				}
			}
		}
		// Remove unstable regions from the graph
		RemoveExpiredNodes(graph);
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::AddNeighboringLayer(NodePointerType n,
														  GraphType& g,
														  std::set<long unsigned int>& bset)
	{
		typedef std::pair<long unsigned int, NodePointerType> KeyValue;
		
		// First we add its neighboring nodes
		for (EdgeIterator eit = n->m_Edges.begin();
			 eit != n->m_Edges.end(); ++eit)
		{
			if(bset.find(eit->m_Target->m_Id) == bset.end())
			{
				g.m_Nodes.push_back(eit->m_Target);
				bset.insert(eit->m_Target->m_Id);
			}

			// We explore the neighboring nodes of this neighboring nodes
			for (EdgeIterator neit = eit->m_Target->m_Edges.begin();
				 neit != eit->m_Target->m_Edges.end(); ++neit)
			{
				if(bset.find(neit->m_Target->m_Id) == bset.end())
				{
					g.m_Nodes.push_back(neit->m_Target);
					bset.insert(neit->m_Target->m_Id);
				}	
			}
		}
	}

	template<class TSegmenter>
	bool GraphOperations<TSegmenter>::IsOnTileBorder(const unsigned int pixelRow,
													 const unsigned int pixelCol,
													 const unsigned int lowerRow,
													 const unsigned int upperRow,
													 const unsigned int lowerCol,
													 const unsigned int upperCol,
													 const unsigned int imageWidth,
													 const unsigned int imageHeight)
	{
		long int rowBounds[2];
		long int colBounds[2];

		rowBounds[0] = ((lowerRow > 0 ) ? lowerRow : -1);
		rowBounds[1] = ((upperRow < imageHeight - 1) ? upperRow : -1);
		colBounds[0] = ((lowerCol > 0) ? lowerCol : -1);
		colBounds[1] = ((upperCol < imageWidth - 1) ? upperCol : -1);

		for(int i = 0; i < 2; i++)
		{
			if(rowBounds[i] > -1 && pixelRow == rowBounds[i] && pixelCol >= lowerCol && pixelCol <= upperCol)
				return true;
		}

		for(int i = 0; i < 2; i++)
		{
			if(colBounds[i] > -1 && pixelCol == colBounds[i] && pixelRow >= lowerRow && pixelRow <= upperRow)
				return true;
		}

		return false;
	}

	template<class TSegmenter>
	bool GraphOperations<TSegmenter>::IsOnTileBorderAndAdjacentTileBorder(const unsigned int pixelRow,
																		  const unsigned int pixelCol,
																		  const unsigned int lowerRow,
																		  const unsigned int upperRow,
																		  const unsigned int lowerCol,
																		  const unsigned int upperCol,
																		  const unsigned int imageWidth,
																		  const unsigned int imageHeight)
	{
		long int rowBounds[4];
		long int colBounds[4];

		if(lowerRow > 0)
		{
			rowBounds[0] = lowerRow - 1;
			rowBounds[1] = lowerRow;
		}
		else
		{
			rowBounds[0] = -1;
			rowBounds[1] = -1;
		}

		if(upperRow < imageHeight - 1)
		{
			rowBounds[2] = upperRow;
			rowBounds[3] = upperRow + 1;
		}
		else
		{
			rowBounds[2] = -1;
			rowBounds[3] = -1;
		}

		if(lowerCol > 0)
		{
			colBounds[0] = lowerCol - 1;
			colBounds[1] = lowerCol;
		}
		else
		{
			colBounds[0] = -1;
			colBounds[1] = -1;
		}

		if(upperCol < imageWidth - 1)
		{
			colBounds[2] = upperCol;
			colBounds[3] = upperCol + 1;
		}
		else
		{
			colBounds[2] = -1;
			colBounds[3] = -1;
		}

		for(int i = 0; i < 4; i++)
		{
			if(rowBounds[i] > -1 && pixelRow == rowBounds[i])
				return true;
		}

		for(int i = 0; i < 4; i++)
		{
			if(colBounds[i] > -1 && pixelCol == colBounds[i])
				return true;
		}

		return false;
	}

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::BuildStabilityMargins(const GraphType& graph,
															GraphType& subgraph,
															const unsigned int lowerRow,
															const unsigned int upperRow,
															const unsigned int lowerCol,
															const unsigned int upperCol,
															const unsigned int imageWidth,
															const unsigned int imageHeight)
	{	
		std::set<long unsigned int> used_regions;
		typedef typename std::set<long unsigned int>::iterator NIterator;

		
		for(NodeConstIterator nit = graph.m_Nodes.begin();
			nit != graph.m_Nodes.end(); ++nit)
		{

			if(IsBboxStrictlyInsideTile(*nit, lowerRow, upperRow, lowerCol, upperCol))
				continue;
			else
			{
				unsigned int pix_col, pix_row;
				ContourOperations::PixelList pixels = ContourOperations::GeneratePixels((*nit)->m_Id,
																						(*nit)->m_Contour,
																						(*nit)->m_Bbox,
																						imageWidth);

				bool is_border = false;
				for (ContourOperations::PixelConstIterator pit = pixels.begin();
					 pit != pixels.end(); ++pit)
				{
					pix_col = (*pit) % imageWidth;
					pix_row = (*pit) / imageWidth;

					if(IsOnTileBorder(pix_row, pix_col, lowerRow, upperRow,
									  lowerCol, upperCol, imageWidth, imageHeight))
					{
						is_border = true;
						if(used_regions.find((*nit)->m_Id) == used_regions.end())
						{
							subgraph.m_Nodes.push_back(*nit);
							used_regions.insert((*nit)->m_Id);
						}
						
						AddNeighboringLayer(*nit, subgraph, used_regions);
					}

					if(is_border)
						break;
				}
			}
		}

		// We remove the useless targeted regions
		for(NodeIterator nit = subgraph.m_Nodes.begin(); nit != subgraph.m_Nodes.end(); ++nit)
		{
			EdgeIterator ret = std::remove_if((*nit)->m_Edges.begin(), (*nit)->m_Edges.end(), IsInSet(used_regions));
			(*nit)->m_Edges.erase(ret, (*nit)->m_Edges.end());
		}
	}

	template<class TSegmenter>
	bool GraphOperations<TSegmenter>::IsBboxStrictlyInsideTile(NodePointerType n,
															   const unsigned int lowerRow,
															   const unsigned int upperRow,
															   const unsigned int lowerCol,
															   const unsigned int upperCol)
	{
		return (n->m_Bbox.m_UX > lowerCol &&
				n->m_Bbox.m_UY > lowerRow &&
				(n->m_Bbox.m_W + n->m_Bbox.m_UX - 1) < upperCol &&
				(n->m_Bbox.m_H + n->m_Bbox.m_UY - 1) < upperRow);
	}

	template<class TSegmenter>
	bool GraphOperations<TSegmenter>::IsBboxInsideTile(NodePointerType n,
													   const unsigned int lowerRow,
													   const unsigned int upperRow,
													   const unsigned int lowerCol,
													   const unsigned int upperCol)
	{
		return (n->m_Bbox.m_UX >= lowerCol &&
				n->m_Bbox.m_UY >= lowerRow &&
				(n->m_Bbox.m_W + n->m_Bbox.m_UX - 1) <= upperCol &&
				(n->m_Bbox.m_H + n->m_Bbox.m_UY - 1) <= upperRow);
	}
	

	template<class TSegmenter>
	void GraphOperations<TSegmenter>::MergeGraphs(GraphType& graph,
												  const unsigned int lowerRow,
												  const unsigned int upperRow,
												  const unsigned int lowerCol,
												  const unsigned int upperCol,
												  const unsigned int imageWidth,
												  const unsigned int imageHeight,
												  bool useless)
	{	
		{
			std::map<long unsigned int, std::vector<NodePointerType> > border_map;
			typedef typename std::map<long unsigned int, std::vector<NodePointerType> >::iterator BorderMapIterator;
		
			// First step : detect the duplicated nodes
		
			for(NodeConstIterator nit = graph.m_Nodes.begin();
				nit != graph.m_Nodes.end(); ++nit)
			{
				if(!IsBboxStrictlyInsideTile(*nit, lowerRow, upperRow, lowerCol, upperCol))
				{	
					unsigned int pix_col, pix_row;
					ContourOperations::PixelList pixels = ContourOperations::GenerateBorderPixels((*nit)->m_Id,
																								  (*nit)->m_Contour,
																								  imageWidth);
					
					for (ContourOperations::PixelConstIterator pit = pixels.begin();
						 pit != pixels.end(); ++pit)
					{
						pix_col = (*pit) % imageWidth;
						pix_row = (*pit) / imageWidth;

						if(IsOnTileBorderAndAdjacentTileBorder(pix_row, pix_col, lowerRow, upperRow,
															   lowerCol, upperCol, imageWidth, imageHeight))
						{	
							if(border_map.find(*pit) != border_map.end())
							{
								NodeIterator is_here =
									std::find_if(border_map[*pit].begin(), border_map[*pit].end(), NodePtrComparator(*nit));
								if(is_here == border_map[*pit].end())
									border_map[*pit].push_back(*nit);
							}
							else
								border_map[*pit].push_back(*nit);
						}
					}
				}
			}

			// Second step : remove the duplicated regions
			for(BorderMapIterator bmit = border_map.begin(); bmit != border_map.end(); ++bmit)
			{
				if(bmit->second.size() > 1) // More than one node ?
				{
					NodePointerType ref = bmit->second.front();

					// Explore the other duplicated nodes
					for(NodeIterator nit = bmit->second.begin() + 1; nit != bmit->second.end(); ++nit)
					{
						// Explore their edges
						for(EdgeIterator eit = (*nit)->m_Edges.begin(); eit != (*nit)->m_Edges.end(); ++eit)
						{
							// We find the edge targeting the duplicated node and we remove it
							EdgeIterator toDoublon = FindEdge(eit->m_Target, *nit);
							assert(toDoublon != eit->m_Target->m_Edges.end());
							unsigned int boundary = toDoublon->m_Boundary;
							eit->m_Target->m_Edges.erase(toDoublon);

							EdgeIterator RefToTarget = FindEdge(ref, eit->m_Target);
							if(RefToTarget == ref->m_Edges.end())
							{	
								// We create a new edge between the reference and the target
								EdgeType edge;
								edge.m_Boundary = boundary;
								edge.m_Cost = 0.0f;
								edge.m_Target = ref;
								eit->m_Target->m_Edges.push_back(edge);
								edge.m_Target = eit->m_Target;
								ref->m_Edges.push_back(edge);
							}
						}
						(*nit)->m_Expired = true;
					}
				
					ContourOperations::PixelList pixels = ContourOperations::GenerateBorderPixels(ref->m_Id,
																								  ref->m_Contour, 
																								  imageWidth);
					for (ContourOperations::PixelConstIterator pit = pixels.begin();
						 pit != pixels.end(); ++pit)
					{
						if(border_map.find(*pit) != border_map.end())
						{
							border_map[*pit].clear();
							border_map[*pit].push_back(ref);
						}
					}
				}
			}
			RemoveExpiredNodes(graph);

			// Update the border regions
			{
				unsigned int boundary;
				for(BorderMapIterator bmit = border_map.begin(); bmit != border_map.end(); ++bmit)
				{
					long int neighborhood[4];
					FOURNeighborhood(neighborhood, bmit->first, imageWidth, imageHeight);
					for(short j = 0; j < 4; ++j)
					{
						if(neighborhood[j] > -1)
						{
							BorderMapIterator isInMap = border_map.find(neighborhood[j]);
							if(isInMap != border_map.end())
							{
								NodePointerType curr_region = bmit->second.front();
								NodePointerType target_region = isInMap->second.front();
								if(curr_region != target_region)
								{
									EdgeIterator ToTarget = FindEdge(curr_region, target_region);
									if(ToTarget == curr_region->m_Edges.end())
									{
										boundary = 0;
										ContourOperations::PixelList pixels =
											ContourOperations::GenerateBorderPixels(curr_region->m_Id,
																					curr_region->m_Contour, 
																					imageWidth);
									
										for (ContourOperations::PixelConstIterator pit = pixels.begin();
											 pit != pixels.end(); ++pit)
										{
											if(border_map.find(*pit) != border_map.end())
											{
												long int curr_neighborhood[4];
												FOURNeighborhood(curr_neighborhood, *pit, imageWidth, imageHeight);
												for(short k = 0; k<4; ++k)
												{
													if(curr_neighborhood[k] > -1)
													{
														BorderMapIterator Detect = border_map.find(curr_neighborhood[k]);
														if(Detect != border_map.end() &&
														   Detect->second.front() == target_region)
															boundary++;
													}
												}
											}
										}

										EdgeType edge;
										edge.m_Boundary = boundary;
										edge.m_Cost = 0.0f;
										edge.m_Target = target_region;
										curr_region->m_Edges.push_back(edge);
										edge.m_Target = curr_region;
										target_region->m_Edges.push_back(edge);
									}
								}
							}
						}
					}
				}
			}
		}

		/*for(NodeConstIterator nit = graph.m_Nodes.begin();
			nit != graph.m_Nodes.end(); ++nit)
		{
			if((*nit)->m_Id == 577)
			{
				std::cout << (*nit)->m_Id << ": ";
					for(EdgeIterator eit = (*nit)->m_Edges.begin(); eit != (*nit)->m_Edges.end(); ++eit)
					{
						std::cout << eit->m_Target->m_Id << " " << eit->m_Boundary << "\t";
					}
					std::cout << std::endl;
			}
			}*/
			
		// Remove useless nodes
		if(useless)
		{
			std::set<NodePointerType> border_nodes;
			std::set<NodePointerType> margin_nodes;
			typedef typename std::set<NodePointerType>::iterator SetIterator;

			// Retrieve the border nodes
			for(NodeIterator nit = graph.m_Nodes.begin(); nit != graph.m_Nodes.end(); ++nit)
			{	
				if(!IsBboxStrictlyInsideTile(*nit, lowerRow, upperRow, lowerCol, upperCol))
				{
					unsigned int pix_col, pix_row;
					ContourOperations::PixelList pixels = ContourOperations::GenerateBorderPixels((*nit)->m_Id,
																								  (*nit)->m_Contour,
																								  imageWidth);
					
					
					for (ContourOperations::PixelConstIterator pit = pixels.begin();
						 pit != pixels.end(); ++pit)
					{
						pix_col = (*pit) % imageWidth;
						pix_row = (*pit) / imageWidth;

						if(IsOnTileBorder(pix_row, pix_col, lowerRow, upperRow,
										  lowerCol, upperCol, imageWidth, imageHeight))
						{
							border_nodes.insert(*nit);
							break;
						}
						else
							continue;
					}
				}
			}

			// Add the margin nodes of the border nodes
			for(SetIterator sit = border_nodes.begin(); sit != border_nodes.end(); ++sit)
			{
				for(EdgeIterator eit = (*sit)->m_Edges.begin(); eit != (*sit)->m_Edges.end(); ++eit)
				{
					if(margin_nodes.find(eit->m_Target) == margin_nodes.end())
					{
						margin_nodes.insert(eit->m_Target);
					}

					for(EdgeIterator eeit = eit->m_Target->m_Edges.begin(); eeit != eit->m_Target->m_Edges.end(); ++eeit)
					{
						if(margin_nodes.find(eeit->m_Target) == margin_nodes.end())
							margin_nodes.insert(eeit->m_Target);
					}
				}
			}

			for(NodeIterator nit = graph.m_Nodes.begin(); nit != graph.m_Nodes.end(); ++nit)
			{	
				if(!IsBboxInsideTile(*nit, lowerRow, upperRow, lowerCol, upperCol) &&
				   margin_nodes.find(*nit) == margin_nodes.end())
				{
					// We remove the edges of the adjacent nodes tageting this node
					for(EdgeIterator eit = (*nit)->m_Edges.begin(); eit != (*nit)->m_Edges.end(); ++eit)
					{
						EdgeIterator ret = FindEdge(eit->m_Target, *nit);
						eit->m_Target->m_Edges.erase(ret);
					}

					// This node has to be removed
					(*nit)->m_Expired = true;
				}
			}

			RemoveExpiredNodes(graph);
		}

		/*for(NodeIterator nit = graph.m_Nodes.begin(); nit != graph.m_Nodes.end(); ++nit)
		{
			std::cout << (*nit)->m_Id << " : ";
			for(EdgeIterator eit = (*nit)->m_Edges.begin(); eit != (*nit)->m_Edges.end(); ++eit)
			{
				std::cout << eit->m_Target->m_Id << " " << eit->m_Boundary << "\t";
			}
			std::cout <<  std::endl;
			}*/

		// Remove useless regions
		
	}

	template<class TSegmenter>
	long long unsigned int GraphOperations<TSegmenter>::GetMemorySpace(const GraphType& graph)
	{
		long long unsigned int res = graph.m_Nodes.size() * NodeSize;

		for(NodeConstIterator nit = graph.m_Nodes.begin(); nit != graph.m_Nodes.end(); ++nit)
		{
			res += ((*nit)->m_Edges.size() * EdgeSize);
		}

		return res;
	}

} // end of namespace lsrm

#endif





