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
#ifndef __GRM_SPRING_SEGMENTER_TXX
#define __GRM_SPRING_SEGMENTER_TXX
#include <otbImageFileReader.h>
#include <itkImageRegionIterator.h>

namespace grm
{

	template<class TImage>
	void
	SpringSegmenter<TImage>::InitFromImage()
	{
		typedef itk::ImageRegionIterator<TImage> ImageIterator;

		this->m_ImageWidth = this->m_InputImage->GetLargestPossibleRegion().GetSize()[0];
		this->m_ImageHeight =this->m_InputImage->GetLargestPossibleRegion().GetSize()[1];
		this->m_NumberOfComponentsPerPixel = this->m_InputImage->GetNumberOfComponentsPerPixel();

		std::size_t idx = 0;
		ImageIterator it(this->m_InputImage, this->m_InputImage->GetLargestPossibleRegion());
		for(it.GoToBegin(); !it.IsAtEnd(); ++it)
		{
			this->m_Graph.m_Nodes[idx]->m_Means.reserve(this->m_NumberOfComponentsPerPixel);

			for(std::size_t b = 0; b < this->m_NumberOfComponentsPerPixel; ++b)
			{
				this->m_Graph.m_Nodes[idx]->m_Means.push_back(it.Get()[b]);
			}	
			++idx;
		}
	}

	template<class TImage>
	float
	SpringSegmenter<TImage>::ComputeMergingCost(NodePointerType n1, NodePointerType n2)
	{
		float eucDist = 0.0;

		for(unsigned int b = 0; b < this->m_NumberOfComponentsPerPixel; b++)
		{
			eucDist += (n1->m_Means[b] - n2->m_Means[b])*(n1->m_Means[b] - n2->m_Means[b]);
		}

		return (static_cast<float>(std::sqrt(eucDist)));
	}

	template<class TImage>
	void
	SpringSegmenter<TImage>::UpdateSpecificAttributes(NodePointerType n1, NodePointerType n2)
	{
		const float a1 = static_cast<float>(n1->m_Area);
		const float a2 = static_cast<float>(n2->m_Area);
		const float a_sum = a1 + a2;

		for(unsigned int b = 0; b < this->m_NumberOfComponentsPerPixel; ++b)
		{
			n1->m_Means[b] = (a1 * n1->m_Means[b] + a2 * n2->m_Means[b]) / a_sum;
		}
	}
} // end of namespace grm
#endif
