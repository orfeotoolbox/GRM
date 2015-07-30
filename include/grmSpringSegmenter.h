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
#ifndef __GRM_SPRING_SEGMENTER_H
#define __GRM_SPRING_SEGMENTER_H
#include "grmSegmenter.h"

namespace grm
{
	struct SpringNode : Node<SpringNode>
	{
		std::vector<float> m_Means;
	};

	struct SpringParam{};

	template<class TImage>
	class SpringSegmenter : public Segmenter< TImage, SpringNode, SpringParam>
	{
	public:
		/* Some convenient typedefs */
		typedef Segmenter<TImage, SpringNode, SpringParam> Superclass;
		typedef TImage ImageType;
		typedef typename Superclass::GraphType GraphType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
		void InitFromImage();
	};
} // end of namespace grm
#include "grmSpringSegmenter.txx"
#endif
