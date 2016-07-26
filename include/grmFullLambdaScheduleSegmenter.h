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
#ifndef GRM_FULL_LAMBDA_SCHEDULE_SEGMENTER_H
#define GRM_FULL_LAMBDA_SCHEDULE_SEGMENTER_H
#include "grmSegmenter.h"

namespace grm
{
	struct FLSNode : Node<FLSNode>
	{
		std::vector<float> m_Means;
	};

	struct FLSParam{};

	template<class TImage>
	class FullLambdaScheduleSegmenter : public Segmenter< TImage, FLSNode, FLSParam>
	{
	public:
		/* Some convenient typedefs */
		typedef Segmenter<TImage, FLSNode, FLSParam> Superclass;
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
#include "grmFullLambdaScheduleSegmenter.txx"
#endif