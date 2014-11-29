/*=========================================================================

  Program: Generic Region Merging Library (GRM)
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __GRM_ALGORITHM_H
#define __GRM_ALGORITHM_H

#include "macro-generator.h"
#include "grm-interface.h"

namespace grm
{
	template<class TInputImage, class TRegion, class TParams>
	class RegionMergingAlgorithm
	{
		public:

			// Mandatory typedef.
			typedef TRegion RegionType;
			typedef std::shared_ptr<RegionType> RegionPointerType;
			typedef std::vector<RegionPointerType> RegionListType;
			typedef TParams ParameterType;
			typedef Interface<RegionType> InterfaceType;
			typedef typename InterfaceType::MergingCostFunction MergingCostFunction;

			// Empty constructor and destructor
			RegionMergingAlgorithm(){}
			virtual ~RegionMergingAlgorithm(){}

			// Abstract method
			virtual void InitFromImage() = 0;
			virtual void UpdateAttribute(RegionPointerType, RegionPointerType) = 0;
			virtual void Segmentation() = 0;

			// No abstract method
			//virtual void WriteLabelOutputImage();
			virtual void WriteRGBOutputImage();

			// Get / Set methods
			GetMacro(RegionListType, RegionList);
			GetMacro(ParameterType, Parameters);
			GetMacro(unsigned int, NumberOfIterations);

			SetMacro(RegionListType, RegionList);
			SetMacro(ParameterType, Parameters);
			SetMacro(std::string, Input);
			SetMacro(std::string, OutputLabel);
			SetMacro(std::string, OutputRGB);
			SetMacro(unsigned int, NumberOfIterations);
			void SetDimensionForEncoder(unsigned int, unsigned int);

		protected:

			virtual void InternalInit(unsigned int, unsigned int);

			// Input / Output attributes
			std::string m_Input;
			std::string m_OutputLabel;
			std::string m_OutputRGB;

			// Algorithm attributes
			RegionListType m_RegionList;
			ParameterType m_Parameters;

			// Algorithm helper
			unsigned int m_NumberOfIterations;
			InterfaceType m_RMHandler;
	};
} // end of namespace grm
#include "grm-algorithm.txx"
#endif
