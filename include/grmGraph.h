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
#ifndef GRM_GRAPH_H
#define GRM_GRAPH_H
#include "grmDataStructures.h"
#include "lpContour.h"

namespace grm
{
	struct BaseNode
	{
		/* Node already merged. */
		bool m_Valid;
		
		/* Node has to be removed from the graph. */
		bool m_Expired;

		/* Does the node merge at the previous iteration */
		bool m_IsMerged;
		
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
		lp::BoundingBox m_Bbox;

		/* Contour of the shape */
		lp::Contour m_Contour;
	};

	template<class DerivedNode>
		struct NeighborType
		{
			typedef std::weak_ptr<DerivedNode> WeakDerived;
			typedef std::shared_ptr<DerivedNode> SharedDerived;

			WeakDerived m_Target;
			float  m_Cost;
			unsigned int m_Boundary;
			bool m_CostUpdated;

		    NeighborType(WeakDerived ptr, double w, unsigned int c) :
			m_Target(ptr), m_Cost(w), m_Boundary(c), m_CostUpdated(false) {}
			
			inline SharedDerived GetRegion()
				{
					SharedDerived ptr(m_Target.lock());
					if(!ptr)
						throw std::runtime_error("NeighborType::GetRegion - Region pointer is not valid");
					return ptr;
				}
		};
	

	template<class DerivedNode>
		struct Node : BaseNode
	{
		typedef NeighborType<DerivedNode> CRPTNeighborType;
		std::vector<CRPTNeighborType> m_Edges;
	};

	template<class TNode>
	struct Graph
	{
		typedef TNode NodeType;
		typedef std::shared_ptr<NodeType> NodePointerType;
		typedef typename NodeType::CRPTNeighborType EdgeType;
		typedef std::vector<NodePointerType> NodeListType;
		typedef typename NodeListType::iterator NodeIteratorType;
		typedef typename NodeListType::const_iterator NodeConstIteratorType;
		typedef std::vector<EdgeType> EdgeListType;
		typedef typename EdgeListType::iterator EdgeIteratorType;
		typedef typename EdgeListType::const_iterator EdgeConstIteratorType;
		
		std::vector< NodePointerType > m_Nodes; 
	};
	
} // end of namespace grm
#endif