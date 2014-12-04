/*=========================================================================

  Program: Implementation of the euclidean distance criterion using the GRM library
  Language: C++
  author: Lassalle Pierre



  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __EUC_DIST_ALGORITHM_TXX
#define __EUC_DIST_ALGORITHM_TXX

#include "euc-dist-algorithm.h"
#include <otbVectorImage.h>
#include <otbImageFileReader.h>
#include <itkImageRegionIterator.h>

template<class TInputImage>
void EuclideanDistanceRM<TInputImage>::InitFromImage()
{
	typedef otb::ImageFileReader<TInputImage> InputImageFileReaderType;
	typedef itk::ImageRegionIterator<TInputImage> InputImageIteratorType;

	// Read the input image
	auto input_reader = InputImageFileReaderType::New();
	input_reader->SetFileName(this->m_Input);
	input_reader->Update();

	// Retrieve useful information from the input image
	auto regionToProcess = input_reader->GetOutput()->GetLargestPossibleRegion();
	unsigned int rows = regionToProcess.GetSize()[1];
	unsigned int cols = regionToProcess.GetSize()[0];
	unsigned int bands = input_reader->GetOutput()->GetNumberOfComponentsPerPixel();

	// Step 1: For efficiency the user is advised to allocate the initial number of regions
	this->m_RegionList.reserve(rows*cols);

	// Step 2: the user has to intialize the specific attributes of the initial regions
	InputImageIteratorType it(input_reader->GetOutput(), regionToProcess);
	for(it.GoToBegin(); !it.IsAtEnd(); ++it)
	{
		auto new_region = std::make_shared<EucDistRegion>();
		new_region->m_Area = 1;
		new_region->m_Means.reserve(bands);
		for(unsigned int b = 0; b<bands; ++b)
			new_region->m_Means.push_back(it.Get()[b]);
		// The user has to push the initial regions
		this->m_RegionList.push_back(new_region);
	}

	// Step 3: The neighborhhod and some internal attributes have to be initialized
	// the user has to call the method InitRegions
	this->InternalInit(rows, cols);
}

// Let assume that regions r1 and r2 have to be merged and r2 is merged into r1
// The user has to describe how the specific attributes of r1 have to be updated
// by defining the mandatory method UpdateAttribute(RegionPointerType, RegionPointerType)
template<class TInputImage>
void 
EuclideanDistanceRM<TInputImage>::UpdateAttribute(RegionPointerType r1, RegionPointerType r2)
{
	float a1 = r1->m_Area, a2 = r2->m_Area, a_tot = a1 + a2;
	for(unsigned int b=0; b<r1->m_Means.size(); ++b)
		r1->m_Means[b] = ((a1*r1->m_Means[b]) + (a2*r2->m_Means[b])) / a_tot;
	r1->m_Area += r2->m_Area;
}

// This method is obviously mandatory but its implementation depends on some
// subtilities of your algorithm
// What is common is the the iterative merging process of the region, what can be different
// is the stopping criterion and the behavior of the threshold
template<class TInputImage>
void 
EuclideanDistanceRM<TInputImage>::Segmentation()
{
	unsigned int final_thresh = static_cast<unsigned int>(this->m_Parameters.m_FinalThresh);
	float curr_threshold;
	bool has_merged;

	MergingCostFunction cost_func = [](RegionPointerType r1, RegionPointerType r2)->double
	{
		double cost = 0.0;
		unsigned int bands = r1->m_Means.size();
		for(unsigned int b = 0; b < bands; b++)
			cost += pow((r1->m_Means[b] - r2->m_Means[b]), 2);
		return cost;
	};

	while(this->m_Parameters.m_CurrThresh < this->m_Parameters.m_FinalThresh)
	{
		has_merged = true;
		curr_threshold = static_cast<float>(this->m_Parameters.m_CurrThresh * this->m_Parameters.m_CurrThresh);
		while(has_merged)
		{
			has_merged = false;
			this->m_RMHandler.UpdateMergingCosts(this->m_RegionList, cost_func);
			std::cout << "." << std::flush;

			// Do a loop on the regions of the graph
			for(auto& r : this->m_RegionList)
			{
				// For each explored region, we determine if the LMBF is met
				auto ref_region = this->m_RMHandler.CheckLMBF(r, curr_threshold);
				// If it holds then the pointer to ref_region is not null
				if(ref_region != nullptr)
				{
					// Process to merge the regions
					// Step 1: Merge the specific attributes
					UpdateAttribute(ref_region, ref_region->GetClosestNeighbor());
					// Step 2: Internal update (mandatory)
					this->m_RMHandler.Update(ref_region, ref_region->GetClosestNeighbor());

					has_merged = true;
				}
			}

			// After one iteration, you have to remove the expired regions, i.e. regions
			// who have merged into a larger one
			this->m_RMHandler.RemoveExpiredVertices(this->m_RegionList);
		}

		++this->m_Parameters.m_CurrThresh;
	}
	std::cout << "\n";
}

#endif
