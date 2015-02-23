#ifndef __LSRM_GRAPH_OPERATIONS_H
#define __LSRM_GRAPH_OPERATIONS_H
#include "lsrmGraph.h"
#include "lsrmNeighborhood.h"
#include "lsrmContourOperations.h"
#include <iostream>
#include <cassert>
#include <limits>
#include <map>
#include <utility>
#include <set>

namespace lsrm
{
	template<class TSegmenter>
	class GraphOperations
	{
	public:
		
		/* Some convenient typedefs */
		typedef TSegmenter SegmenterType;
		typedef typename SegmenterType::GraphType GraphType;
		typedef typename GraphType::NodeType NodeType;
		typedef typename GraphType::EdgeType EdgeType;
		typedef typename GraphType::NodePointerType NodePointerType;
		typedef typename GraphType::NodeListType NodeList;
		typedef typename GraphType::NodeIteratorType NodeIterator;
		typedef typename GraphType::NodeConstIteratorType NodeConstIterator;
		typedef typename GraphType::EdgeListType EdgeList;
		typedef typename GraphType::EdgeIteratorType EdgeIterator;
		typedef typename GraphType::EdgeConstIteratorType EdgeConstIterator;

		/* Some static constants */
		static const long long unsigned int NodeSize = sizeof(NodeType) + sizeof(NodePointerType);
		static const long long unsigned int EdgeSize = sizeof(EdgeType);

		/* Functors (former than lambda expression...
		   but it can accept all compiler' versions) */
		struct EdgeEqualTo
		{
			explicit EdgeEqualTo(NodePointerType t_) : m_T(t_)
				{assert(m_T != NULL);}
			
			bool operator()(const EdgeType& e)
				{
					return e.m_Target == m_T;
				}

			NodePointerType  m_T;
		};

		struct IsExpired
		{
			IsExpired () {}
			bool operator()(NodePointerType  n) const
				{
					return n->m_Expired;
				}
		};

		struct IsInSet
		{
			explicit IsInSet(std::set<long unsigned int>& ur) : m_UsedRegions(ur)
				{
					
				}

			bool operator()(const EdgeType& e)
				{
					if(m_UsedRegions.find(e.m_Target->m_Id) == m_UsedRegions.end())
						return true;
					else
						return false;
				}
				
			std::set<long unsigned int>& m_UsedRegions;
		};

		struct NodePtrComparator : public std::unary_function<NodePointerType, bool>
		{
			NodePointerType nn;
			explicit NodePtrComparator(NodePointerType n) : nn(n){}
			bool operator()(NodePointerType v) { return v == nn;}
		};
		

		/*
		 * Given the size of the input image and the mask of the
		 * neighborhood, we initialize a new graph of nodes
		 *
		 * @params:
		 * GraphType& graph: reference to a graph of nodes
		 * const unsigned int width: width of the input image
		 * const unsigned int height: height of the input image
		 * CONNECTIVITY mask : mask of the neighborhood (4X4 or 8X8)
		 */
		static void InitNodes(GraphType& graph,
							  SegmenterType& seg,
							  const std::string& inputFileName,
							  CONNECTIVITY mask);

		/*
		 * Given a graph of nodes, we explore all the nodes
		 * and for each node we compute his merging costs
		 * with all its neighboring nodes given a function
		 * to compute the merging cost between two nodes.
		 *
		 * @params:
		 * GraphType& graph: reference to the graph of nodes
		 * float(*fptr)(NodeType*, NodeType*): pointer to the function
		 * to compute the merging cost between two adjacent nodes.
		 */
		static void UpdateMergingCosts(GraphType& graph, SegmenterType& seg);

		/*
		 * Given a node A, we analyse its best node B.
		 * If the node A is also node B's best node
		 * then it returns a pointer to node B if node A 's id
		 * is smaller or a pointer to node A if node B's id is
		 * smaller
		 * else it returns a null pointer.
		 * (Local Mutual Best Fitting Heuristic)
		 * 
		 * @params:
		 * NodeType * a : Pointer to the node A
		 * float t : threshold of the criterion
		 */
		static NodePointerType CheckLMBF(NodePointerType, float t);

		/*
		 * Given a node A, we analyse its best node B.
		 * If the criterion is checked and the node B
		 * is valid then it returns a pointer to the
		 * node A if node's A id is smaller or a pointer
		 * to node B if node B's id is smaller
		 * else it returns a null pointer.
		 *
		 * @params:
		 * NodeType * a : pointer to node A
		 * float t : threshold of the criterion
		 */
		static NodePointerType CheckBF(NodePointerType a, float t);

		/*
		 * Given the current node and the target node, it returns
		 * the edge from the current node targeting to the target
		 * node.
		 *
		 * @params
		 * const NodeType * n : pointer to the current node.
		 * const NodeType * target : pointer to the target node.
		 * @return an iterator pointing to the candidate edge.
		 */
		static EdgeIterator FindEdge(NodePointerType n, NodePointerType target);

		/*
		 * Given a node a and the node b to be merged into node a,
		 * it updates the neighbors of node a with respect to the
		 * neighbors of node b.
		 *
		 * @params
		 * NodeType * a : pointer to node a.
		 * NodeType * b : pointer to node b.
		 */
		static void UpdateNeighbors(NodePointerType a, NodePointerType b);
		
		/*
		 * Given 2 nodes A and B (node B being merged into node A)
		 * we update the internal attributes of node A with respect
		 * to node B.
		 *
		 * @params:
		 * NodeType * a: pointer to node A.
		 * NodeType * b: pointer to node B.
		 */
		static void UpdateInternalAttributes(NodePointerType a,
											 NodePointerType b,
											 const unsigned int width,
											 const unsigned int height);

		/*
		 * Given a graph, it removes all the expired nodes.
		 *
		 * @params
		 * GraphType& graph : reference to the graph.
		 */
		static void RemoveExpiredNodes(GraphType& graph);


		/*
		 * Given a graph, a region merging algorithm, a threshold
		 * and the dimension of the image, it performs one iteration
		 * of the merging process using the local mutual best fitting
		 * heuristic.
		 *
		 * @params
		 * GraphType& graph : reference to the graph
		 * SegmenterType& seg : reference to the region merging algorithm.
		 * const float threshold : threshold for this iteration.
		 * const unsigned int width : width of the image.
		 * const unsigned int height : height of the image.
		 *
		 * @return a boolean pointing out if there was at least a fusion
		 * of nodes.
		 */
		static bool PerfomOneIterationWithLMBF(GraphType& graph,
											   SegmenterType& seg,
											   const float threshold,
											   const unsigned int width,
											   const unsigned int height);

		/*
		 * Given a graph, a region merging algorithm, a threshold
		 * and the dimension of the image, it performs one iteration
		 * of the merging process using the best fitting
		 * heuristic.
		 *
		 * @params
		 * GraphType& graph : reference to the graph
		 * SegmenterType& seg : reference to the region merging algorithm.
		 * const float threshold : threshold for this iteration.
		 * const unsigned int width : width of the image.
		 * const unsigned int height : height of the image.
		 *
		 * @return a boolean pointing out if there was at least a fusion
		 * of nodes.
		 */
		static bool PerfomOneIterationWithBF(GraphType& graph,
											 SegmenterType& seg,
											 const float threshold,
											 const unsigned int width,
											 const unsigned int height);

		/*
		 * Given a graph, a region merging algorithm, a threshold,
		 * the number of iterations to apply and the dimension of the image,
		 * it performs all the iterations of the merging process using the
		 * local mutual best fitting heuristic.
		 * This method can be used when the threshold is constant during
		 * the region merging process.
		 *
		 * @params
		 * GraphType& graph : reference to the graph
		 * SegmenterType& seg : reference to the region merging algorithm.
		 * const float threshold : threshold for this iteration.
		 * const unsigned int numberOfIterations: number of iteration to perform.
		 * const unsigned int width : width of the image.
		 * const unsigned int height : height of the image.
		 *
		 * @return a boolean pointing out if there was at least a fusion
		 * of nodes.
		 */
		static bool PerfomAllIterationsWithLMBFAndConstThreshold(GraphType& graph,
																 SegmenterType& seg,
																 const float threshold,
																 const unsigned int numberOfIterations,
																 const unsigned int width,
																 const unsigned int height);

		/*
		 * Given a graph, a region merging algorithm, a threshold,
		 * the number of iterations to apply and the dimension of the image,
		 * it performs all the iterations of the merging process using the
		 * best fitting heuristic.
		 * This method can be used when the threshold is constant during
		 * the region merging process.
		 *
		 * @params
		 * GraphType& graph : reference to the graph
		 * SegmenterType& seg : reference to the region merging algorithm.
		 * const float threshold : threshold for this iteration.
		 * const unsigned int numberOfIterations: number of iteration to perform.
		 * const unsigned int width : width of the image.
		 * const unsigned int height : height of the image.
		 *
		 * @return a boolean pointing out if there was at least a fusion
		 * of nodes.
		 */
		static bool PerfomAllIterationsWithBFAndConstThreshold(GraphType& graph,
															   SegmenterType& seg,
															   const float threshold,
															   const unsigned int numberOfIterations,
															   const unsigned int width,
															   const unsigned int height);

		/*
		 *
		 */
		static void LargeScaleInitNodes(GraphType& graph,
										SegmenterType& seg,
										const unsigned int tileWidth,
										const unsigned int tileHeight,
										const unsigned int nbTilesX,
										const unsigned int nbTilesY,
										const unsigned int tileId,
										const unsigned int margin,
										const unsigned int imageWidth,
										const unsigned int imageHeight,
										CONNECTIVITY mask);

		/*
		 *
		 */
		static bool IsOnTileBorder(const unsigned int pixelRow,
								   const unsigned int pixelCol,
								   const unsigned int lowerRow,
								   const unsigned int upperRow,
								   const unsigned int lowerCol,
								   const unsigned int upperCol,
								   const unsigned int imageWidth,
								   const unsigned int imageHeight);

		static bool IsOnTileBorderAndAdjacentTileBorder(const unsigned int pixelRow,
														const unsigned int pixelCol,
														const unsigned int lowerRow,
														const unsigned int upperRow,
														const unsigned int lowerCol,
														const unsigned int upperCol,
														const unsigned int imageWidth,
														const unsigned int imageHeight);
		/*
		 *
		 */
		static bool IsBboxStrictlyInsideTile(NodePointerType n,
											 const unsigned int lowerRow,
											 const unsigned int upperRow,
											 const unsigned int lowerCol,
											 const unsigned int upperCol);

		/*
		 *
		 */
		static bool IsBboxInsideTile(NodePointerType n,
									 const unsigned int lowerRow,
									 const unsigned int upperRow,
									 const unsigned int lowerCol,
									 const unsigned int upperCol);
		
		/*
		 *
		 */
		static void RemoveEdgesToUnstableRegion(NodePointerType r);
		
		/*
		 *
		 */
		static void RemoveUnstableRegions(GraphType& graph,
										  unsigned int lowerRow,
										  unsigned int upperRow,
										  unsigned int lowerCol,
										  unsigned int upperCol,
										  unsigned int imageWidth);

		/*
		 *
		 */
		static void AddNeighboringLayer(NodePointerType n,
										GraphType& g,
										std::set<long unsigned int>& bset);
		
		/*
		 *
		 */
		static void BuildStabilityMargins(const GraphType& graph,
										  GraphType& subgraph,
										  const unsigned int lowerRow,
										  const unsigned int upperRow,
										  const unsigned int lowerCol,
										  const unsigned int upperCol,
										  const unsigned int imageWidth,
										  const unsigned int imageHeight);

		/*
		 *
		 */
		static void MergeGraphs(GraphType& graph,
								const unsigned int lowerRow,
								const unsigned int upperRow,
								const unsigned int lowerCol,
								const unsigned int upperCol,
								const unsigned int imageWidth,
								const unsigned int imageHeight,
								bool useless);

		/*
		 *
		 */
		static long long unsigned int GetMemorySpace(const GraphType& graph);
	};
} // end of namespace lsrm

#include "lsrmGraphOperations.txx"
#endif



