#ifndef __LSRM_GRAPH_H
#define __LSRM_GRAPH_H
#include <boost/shared_ptr.hpp>
#include "lsrmDataStructures.h"

namespace lsrm
{
	struct BaseNode
	{
		BaseNode(){}
		
		virtual ~BaseNode() {}

		/* Node already merged. */
		bool m_Valid;
		
		/* Node has to be removed from the graph. */
		bool m_Expired;

		/* Perimeter of the region */
		unsigned int m_Perimeter;

		/* Area (number of inner pixels) of the region */
		unsigned int m_Area;

		/*
		  Node is identified by the location
		  of the first pixel of the region.
		 */
		long unsigned int m_Id;

		/*
		  Bounding box of the region
		  in the image.
		 */
		BoundingBox m_Bbox;

		/*
		  Contour of the region
		 */
		Contour m_Contour;
	};

	template<class DerivedNode>
	struct Node : BaseNode
	{
		struct Edge
		{
			/* Boundary length between two adjacent regions.*/
			unsigned int m_Boundary;
			
			/* Fusion cost (similarity measure) with the target node. */
			float m_Cost;
			
			/* Pointer to a neighboring node. */
			boost::shared_ptr<DerivedNode> m_Target;
		};
		
		Node(){};
		virtual ~Node() {}

		std::vector<Edge> m_Edges;
	};

	template<class TNode>
	struct Graph
	{
		typedef TNode NodeType;
		typedef typename NodeType::Edge EdgeType;
		typedef boost::shared_ptr<NodeType> NodePointerType;
		typedef std::vector<NodePointerType> NodeListType;
		typedef typename NodeListType::iterator NodeIteratorType;
		typedef typename NodeListType::const_iterator NodeConstIteratorType;
		typedef std::vector<EdgeType> EdgeListType;
		typedef typename EdgeListType::iterator EdgeIteratorType;
		typedef typename EdgeListType::const_iterator EdgeConstIteratorType;
		
		std::vector< boost::shared_ptr<TNode> > m_Nodes; 
	};
	
} // end of namespace lsrm
#endif


