/*=========================================================================

  Program: Generic Region Merging Library
  Language: C++
  author: Lassalle Pierre
  contact: lassallepierre34@gmail.com



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

     Template Model to help the developper to add a new criterion
     This file as well as his corresponding txx file has to be added
     src folder.

=========================================================================*/
#ifndef __GRM_MYSEGMENTER_H
#define __GRM_MYSEGMENTER_H
#include "grmSegmenter.h"

namespace grm
{
	struct MySpecificNode : Node<MySpecificNode>
	{
		// Specific Attributes for your criterion.
	};

	struct MySpecificParams
	{
		// Specific user-defined parameters for your criterion.
	};
	
	template<class TImage>
	class MySegmenter : public Segmenter< TImage, MySpecificNode, MySpecificParams>
	{
	public:

		/* Some convenient typedefs (put them all) */
		typedef Segmenter<TImage, MySpecificNode, MySpecificParams> Superclass;
		typedef TImage ImageType;
		typedef MySpecificParams ParameterType;
		typedef typename Superclass::GraphType GraphType;
		typedef MySpecificNode NodeType;
		typedef typename Superclass::EdgeType EdgeType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		// Mandatory functions to be defined by the developer.
		void InitFromImage();
		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
	};
	
} // end of namespace grm
#include "GRMSegmenterTemplate.txx"
#endif
