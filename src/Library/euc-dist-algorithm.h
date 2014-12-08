/*=========================================================================

  Program: Implementation of the euclidean distance criterion using the GRM library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __EUC_DIST_ALGORITHM_H
#define __EUC_DIST_ALGORITHM_H

#include "grm-region.h"
#include "grm-algorithm.h"

class EucDistRegion : public grm::RegionBase<EucDistRegion>
{
	public:
		EucDistRegion(){}
		~EucDistRegion(){}

		float m_Area;
		std::vector<float> m_Means;
};

struct EucDistParams
{
	float m_FinalThresh;
	float m_InitialThresh;
};

template<class TInputImage>
class EuclideanDistanceRM : public grm::RegionMergingAlgorithm<TInputImage, 
														EucDistRegion,
														EucDistParams>
{
	public:

		// Mandatory typedef.
		typedef grm::RegionMergingAlgorithm<TInputImage, EucDistRegion, EucDistParams> Superclass;
		typedef EucDistRegion RegionType;
		typedef std::shared_ptr<RegionType> RegionPointerType;
		typedef std::vector<RegionPointerType> RegionListType;
		typedef EucDistParams ParameterType;
		typedef grm::Interface<RegionType> InterfaceType;
		typedef typename Superclass::MergingCostFunction MergingCostFunction;

		EuclideanDistanceRM(){}
		virtual ~EuclideanDistanceRM(){}

		void InitFromImage();
		void UpdateAttribute(RegionPointerType, RegionPointerType);
		virtual void Segmentation();
};
#include "euc-dist-algorithm.txx"
#endif
