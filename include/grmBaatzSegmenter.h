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
#ifndef GRM_BAATZ_SEGMENTER_H
#define GRM_BAATZ_SEGMENTER_H
#include "grmSegmenter.h"

namespace grm
{
	struct BaatzNode : Node<BaatzNode>
	{
		std::vector<float> m_Means;
		std::vector<float> m_SquareMeans;
		std::vector<float> m_SpectralSum;
		std::vector<float> m_Std;
	};

	struct BaatzParam
	{
		float m_SpectralWeight;
		float m_ShapeWeight;
	};
	
	template<class TImage>
	class BaatzSegmenter : public Segmenter< TImage, BaatzNode, BaatzParam>
	{
	public:

		/* Some convenient typedefs */
		typedef Segmenter<TImage, BaatzNode, BaatzParam> Superclass;
		typedef TImage ImageType;
		typedef BaatzParam ParameterType;
		typedef typename Superclass::GraphType GraphType;
		typedef BaatzNode NodeType;
		typedef typename Superclass::EdgeType EdgeType;
		typedef typename Superclass::NodePointerType NodePointerType;
		typedef typename Superclass::GraphOperatorType GraphOperatorType;
		typedef GraphToOtbImage<GraphType> IOType;

		float ComputeMergingCost(NodePointerType n1, NodePointerType n2);
		void UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2);
		void InitFromImage();
	};
} // end of namespace grm
#include "grmBaatzSegmenter.txx"
#endif