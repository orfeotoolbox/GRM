/*=========================================================================

  Program: Implementation of the Full lambda schedule criterion using the GRM library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __FLS_ALGORITHM_H
#define __FLS_ALGORITHM_H

#include "grm-region.h"
#include "grm-algorithm.h"

class FlsRegion : public grm::RegionBase<FlsRegion>
{
	public:
		float m_Area;
		std::vector<float> m_Means;
};

typedef double FLSParams;

template<class TInputImage>
class FLSAlgorithmRM : public grm::RegionMergingAlgorithm<TInputImage, 
														FlsRegion,
														FLSParams>
{
	public:
		// Mandatory typedef
		typedef grm::RegionMergingAlgorithm<TInputImage, FlsRegion, FLSParams> Superclass;
		typedef FlsRegion RegionType;
		typedef std::shared_ptr<RegionType> RegionPointerType;
		typedef std::vector<RegionPointerType> RegionListType;
		typedef FLSParams ParameterType;
		typedef grm::Interface<RegionType> InterfaceType;
		typedef typename Superclass::MergingCostFunction MergingCostFunction;

		FLSAlgorithmRM(){}
		virtual ~FLSAlgorithmRM(){}

		void InitFromImage();
		void UpdateAttribute(RegionPointerType, RegionPointerType);
		virtual void Segmentation();
};
#include "fls-algorithm.txx"
#endif
