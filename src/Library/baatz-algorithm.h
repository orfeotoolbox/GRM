/*=========================================================================

  Program: Implementation of the Baatz & Schäpe criterion using the GRM library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __BAATZ_ALGORITHM_H
#define __BAATZ_ALGORITHM_H

#include "grm-region.h"
#include "grm-algorithm.h"

class BaatzRegion : public grm::RegionBase<BaatzRegion>
{
	public:
		float m_Area;
		float m_Perimeter;
		std::vector<float> m_Means;
		std::vector<float> m_SquareMeans;
		std::vector<float> m_SpectralSum;
		std::vector<float> m_Std;
};

struct BaatzParams
{
	float m_SpectralWeight;
	float m_ShapeWeight;
	float m_Scale;
};

template<class TInputImage>
class BaatzAlgorithmRM : public grm::RegionMergingAlgorithm<TInputImage, 
														BaatzRegion,
														BaatzParams>
{
	public:
		// Mandatory typedef
		typedef grm::RegionMergingAlgorithm<TInputImage, BaatzRegion, BaatzParams> Superclass;
		typedef BaatzRegion RegionType;
		typedef std::shared_ptr<RegionType> RegionPointerType;
		typedef std::vector<RegionPointerType> RegionListType;
		typedef BaatzParams ParameterType;
		typedef grm::Interface<RegionType> InterfaceType;
		typedef typename Superclass::MergingCostFunction MergingCostFunction;

		BaatzAlgorithmRM(){}
		virtual ~BaatzAlgorithmRM(){}

		void InitFromImage();
		void UpdateAttribute(RegionPointerType, RegionPointerType);
		virtual void Segmentation();
};
#include "baatz-algorithm.txx"
#endif
